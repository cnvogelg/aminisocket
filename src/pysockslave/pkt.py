# pkt - packet transfer across vpar port

import vpar
import time

MAGIC=0x42
CRC=0x01
NO_CRC=0x02

class Pkt:
    def __init__(self, vpar):
        self.vpar = vpar
        # first make sure we have updated values - drain data and status channels
        self.vpar.drain()
    
    def wait_select(self, value, timeout=1):
        """wait for SELECT signal"""
        t = time.time()
        end = t + timeout
        while t < end:
            s = self.vpar.peek_status() & vpar.SEL_MASK
            if s and value:
                return True
            elif not s and not value:
                return True
            
            rem = end - t
            self.vpar.read(rem)
            t = time.time()
        return False
    
    def wait_line_toggle(self, expect, timeout=1):
        """wait for toggle on POUT signal"""
        #print expect
        t = time.time()
        end = t + timeout
        while t < end:
            s = self.vpar.peek_status()
            if expect and ((s & vpar.POUT_MASK) == vpar.POUT_MASK):
                return True
            elif (not expect) and ((s & vpar.POUT_MASK) == 0):
                return True
            
            # check for SELECT -> arbitration loss?
            if (s & vpar.SEL_MASK) != vpar.SEL_MASK:
                print "LOST SELECT"
                return False
                
            rem = end - t
            self.vpar.read(rem)
            t = time.time()
        return False
    
    def toggle_busy(self):
        s = self.vpar.get_status()
        if s & vpar.BUSY_MASK:
            n = s & ~vpar.BUSY_MASK
        else:
            n = s | vpar.BUSY_MASK
        self.vpar.set_status(n)
        #print "toggle %02x -> %02x\n" % (s,n)
            
    def set_next_byte(self, toggle_expect, value, timeout=1):
        ok = self.wait_line_toggle(toggle_expect, timeout)
        if not ok:
            print "FAILED: wait for",toggle_expect
            return False
        self.vpar.set_data(value)
        self.toggle_busy()
        return True

    def set_next_word(self, toggle_expect, value, timeout=1):
        v1 = value / 256
        ok = self.set_next_byte(toggle_expect, v1, timeout)
        if not ok:
            return False
        v2 = value % 256
        ok = self.set_next_byte(not toggle_expect, v2, timeout)
        return ok

    def send(self, data, crc=False):
        start = time.time()
        """send a packet via vpar port"""
        # set HS_LINE (BUSY)
        s = self.vpar.get_status() | vpar.BUSY_MASK
        self.vpar.set_status(s)
        # set magic byte
        self.vpar.set_data(MAGIC)
        # pulse ACK -> trigger IRQ in server
        self.vpar.trigger_ack()
        # wait for select
        ok = self.wait_select(True)
        if not ok:
            print "ERROR: Waiting for select"
            return False
        # set byte for CRC
        ok = self.set_next_byte(True, 0x02)
        if not ok:
            print "ERROR: CRC flag"
            return False
        # send size
        size = len(data)
        ok = self.set_next_word(False, size)
        if not ok:
            print "ERROR: size"
            return False    
        # send data
        toggle = False
        pos = 0
        for a in data:
            ok = self.set_next_byte(toggle, ord(a))
            if not ok:
                print "ERROR: data",pos,time.time()-start
                return False
            toggle = not toggle
            pos += 1
        # wait for final toggle
        ok = self.wait_line_toggle(toggle)
        if not ok:
            print "ERROR: final toggle"
        # clear HS_LINE (BUSY)
        s = self.vpar.get_status() & ~vpar.BUSY_MASK
        self.vpar.set_status(s)
        # wait for select end
        ok = self.wait_select(False)
        if not ok:
            print "ERROR: Waiting for unselect"
        return ok
