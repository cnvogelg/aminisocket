# vpar.py - access virtual parallel port of patched FS-UAE

import select

# bit masks for ctl flags
BUSY_MASK = 1
POUT_MASK = 2
SEL_MASK = 4
ACK_MASK = 8

class VPar:
    def __init__(self, ctl_name=None, dat_name=None):
        self.ctl_name = ctl_name
        self.dat_name = dat_name
        self.ctl_file = open(self.ctl_name, "r+b", 0)
        self.dat_file = open(self.dat_name, "r+b", 0)
        self.ctl = 0
        self.dat = 0
    
    def close(self):
        self.ctl_file.close()
        self.dat_file.close()

    def write_data(self, val):
        """write a byte to the parallel port"""
        self.dat_file.write(chr(val))
        self.dat = val

    def has_data(self):
        """poll if new data is available"""
        ready = select.select([self.dat_file],[],[],0)[0]
        return len(ready)

    def read_data(self):
        """read byte from parallel port"""
        # poll port - if something is here read it first
        if self.has_data():
            self.dat = ord(self.dat_file.read(1))
        return self.dat

    def write_status(self, val):
        """write a new status flag value"""
        self.ctl_file.write(chr(val))
        self.ctl = val
    
    def set_status(self, val):
        self.write_status(self.ctl | val)
        
    def clr_status(self, val):
        self.write_status(self.ctl & ~val)
    
    def has_status(self):
        """poll if new status is available"""
        ready = select.select([self.ctl_file],[],[],0)[0]
        return len(ready)>0

    def read_status(self):
        """read status flag values"""
        # poll port
        if self.has_status():
            self.ctl = ord(self.ctl_file.read(1))
        return self.ctl
        
    def wait_data_or_status(self, timeout=0):
        """block until status or data changes. read updated value.
           return (data_available, status_available) bool flag """
        ready = select.select([self.ctl_file, self.dat_file],[],[],timeout)[0]
        got_data = False
        got_status = False
        if self.dat_file in ready:
            self.dat = ord(self.dat_file.read(1))
            got_data = True
        if self.ctl_file in ready:
            self.ctl = ord(self.ctl_file.read(1))
            got_status = True
        return (got_data, got_status)

    def get_data(self):
        return self.dat
    
    def get_status(self):
        return self.ctl
