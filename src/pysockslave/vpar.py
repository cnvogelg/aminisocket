# vpar.py - access virtual parallel port of patched FS-UAE

import select

# bit masks for ctl flags
BUSY_MASK = 1
POUT_MASK = 2
SEL_MASK = 4
ACK_MASK = 8

class VPar:
    def __init__(self, par_name=None):
        self.par_name = par_name
        self.par_file = open(self.par_name, "r+b", 0)
        self.ctl = 0
        self.dat = 0
    
    def close(self):
        self.par_file.close()

    def drain(self):
        """slurp all incoming data"""
        while self.has_data():
            self.par_file.read(1)
            
    def write(self):
        """write status and data"""
        self.par_file.write(chr(self.ctl) + chr(self.dat))
        print("tx: ctl=%02x dat=%02x" % (self.ctl, self.dat))

    def has_data(self, timeout=0):
        """poll if new data is available"""
        ready = select.select([self.par_file],[],[],timeout)[0]
        return len(ready)

    def read(self, timeout=0):
        """read status and data"""
        # poll port - if something is here read it first
        if self.has_data(timeout):
            d = self.par_file.read(2)
            self.ctl = ord(d[0])
            self.dat = ord(d[1])
            print("rx: ctl=%02x dat=%02x" % (self.ctl, self.dat))
            
    def trigger_ack(self):
        self.par_file.write(chr(self.ctl | ACK_MASK) + chr(self.dat))
        print("tx: ctl=%02x dat=%02x" % (self.ctl, self.dat))        

    def set_status(self, val):
        self.ctl = val
        self.write()
        
    def set_data(self, val):
        self.dat = val
        self.write()
        
    def get_status(self, timeout=0):
        self.read(timeout)
        return self.ctl
    
    def get_data(self, timeout=0):
        self.read(timeout)
        return self.dat
        
    def set(self, ctl, dat):
        self.ctl = ctl
        self.dat = dat
        self.write()
        
    def get(self, timeout=0):
        self.read(timeout)
        return (self.ctl, self.dat)
        
    def peek_status(self):
        return self.ctl

