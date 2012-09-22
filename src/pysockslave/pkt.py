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
            self.vpar.clr_status_mask(vpar.BUSY_MASK)
        else:
            self.vpar.set_status_mask(vpar.BUSY_MASK)
            
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

    def get_next_byte(self, toggle_expect, timeout=1):
        ok = self.wait_line_toggle(toggle_expect, timeout)
        if not ok:
            print "FAILED: wait for",toggle_expect
            return None
        data = self.vpar.get_data()
        self.toggle_busy()
        return data
    
    def get_next_word(self, toggle_expect, timeout=1):
        v1 = self.get_next_byte(toggle_expect, timeout)
        if v1 == None:
            return None
        v2 = self.get_next_byte(not toggle_expect, timeout)
        if v2 == None:
            return None
        return v1 * 256 + v2
    
    # ----- public API ----

    def send(self, data, timeout=1, crc=False):
        """send a packet via vpar port"""
        # set HS_LINE (BUSY)
        self.vpar.set_status_mask(vpar.BUSY_MASK)
        # set magic byte
        self.vpar.set_data(MAGIC)
        # pulse ACK -> trigger IRQ in server
        self.vpar.trigger_ack()
        # wait for select
        ok = self.wait_select(True, timeout)
        if not ok:
            print "tx ERROR: Waiting for select"
            return False
        # set byte for CRC
        ok = self.set_next_byte(True, 0x02, timeout)
        if not ok:
            print "tx ERROR: CRC flag"
            return False
        # send size
        size = len(data)
        ok = self.set_next_word(False, size, timeout)
        if not ok:
            print "tx ERROR: size value"
            return False
        # send CRC
        crc = 0
        ok = self.set_next_word(False, crc, timeout)
        if not ok:
            print "tx ERROR: CRC value"
            return False    
        # send data
        toggle = False
        pos = 0
        for a in data:
            ok = self.set_next_byte(toggle, ord(a), timeout)
            if not ok:
                print "tx ERROR: data",pos
                return False
            toggle = not toggle
            pos += 1
        # wait for final toggle
        ok = self.wait_line_toggle(toggle, timeout)
        if not ok:
            print "tx ERROR: final toggle"
        # clear HS_LINE (BUSY)
        self.vpar.clr_status_mask(vpar.BUSY_MASK)
        # wait for select end
        ok = self.wait_select(False, timeout)
        if not ok:
            print "tx ERROR: Waiting for unselect"
        return ok

    def can_recv(self, timeout=0):
        """check if we can receive a packet"""
        # first assume SELECT to be set
        return self.wait_select(True, timeout)

    def recv(self, timeout=1):
        """receive a data packet via vpar port. make sure to call can_recv() before!!"""
        # get magic byte
        magic = self.get_next_byte(True, timeout)
        if not magic:
            print "rx ERROR: no magic"
            return None
        # get crc mode
        crc_mode = self.get_next_byte(False, timeout)
        if not crc_mode:
            print "rx ERROR: no crc mode"
            return None
        if crc_mode < 1 or crc_mode > 2:
            print "rx ERROR: invalid CRC mode",crc_mode
            return None
        # get size
        size = self.get_next_word(True, timeout)
        if size == None:
            print "rx ERROR: no size"
            return None
        # get crc
        crc = self.get_next_word(True, timeout)
        if crc == None:
            print "rx ERROR: no crc value"
            return None
        # get data
        toggle = True
        pos = 0
        data = ""
        for i in xrange(size):
            d = self.get_next_byte(toggle, timeout)
            if d == None:
                print "rx ERROR: data",pos
            data += chr(d)
            toggle = not toggle
            pos = pos+1
        # wait for select gone
        ok = self.wait_select(False, timeout)
        # clear BUSY
        self.vpar.clr_status_mask(vpar.BUSY_MASK)
        if not ok:
            print "rx ERROR: no unselect" 
            return None
        return data

