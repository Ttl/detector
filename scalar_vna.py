import matplotlib.pyplot as plt
import numpy as np
from detector import *
from vna import *
import pickle

lo_set = False
lo_pll = MAX2871(2)
source_pll = MAX2871(3)

def measure(sers, device, freqs, apwr=1):
    global lo_set
    real_freqs = []
    samples = []
    for freq in freqs:
        #Set T_ADJ
        #for ser in sers:
        #    if freq >= 5.3e9:
        #        #500 ohm if f > 5.3 GHz
        #        ser.write('\xf1')
        #    else:
        #        #8.2k if f < 5.3 GHz
        #        ser.write('\xf0')

        source_freq = freq
        ref_freq = 19.2e6

        select_filter(device, source_freq)

        real_freqs.append(source_pll.freq_to_regs(source_freq, ref_freq, apwr=apwr))

        if not lo_set:
            lo_pll.freq_to_regs(source_freq, ref_freq, apwr=0)
            lo_pll.to_device(device)
            time.sleep(20e-3)
            lo_pll.to_device(device)
            lo_set = True
            source_pll.to_device(device)
            time.sleep(20e-3)
        source_pll.to_device(device)

        #print source_power(device, source_freq)

        ps = [None, None]
        for e,ser in enumerate(sers):
            ser.reset_input_buffer()
            while True:
                try:
                    x = ord(ser.read(1))
                    if x != 0xFF:
                        continue
                    x = map(ord,ser.read(2))
                    if x[0] & 0xF0 != 0x00:
                        #Not synchronized, try again
                        continue
                    y = ((x[0] & 0x0F) << 8) | x[1]
                    ps[e] = (v_to_dbm(3.3*y/4095.0, freq))
                except KeyboardInterrupt:
                    print "Exiting"
                    break
                break
        print ps[1]-ps[0],ps[0],ps[1]
        samples.append(ps[1]-ps[0])
    return real_freqs, samples

if __name__ == "__main__":

    #Find VNA to use as a source
    source = None
    device = usb.core.find(idVendor=0x1d50, idProduct=0x6099)
    if device == None:
        print "VNA not found"
        exit(0)
    print "Found VNA"

    device.set_configuration()


    #Initialize VNA
    device.ctrl_transfer(0x40, 4, 0, (1 << 0) | (1 << 2) | (0 << 3)) #Signal, PA on
    write_att(device, 0)
    select_mixer_input(device, 'rx1')
    select_port(device, 2)

    sers = []
    ports = serial.tools.list_ports.comports()
    for dev, name, desc in ports:
        if 'VID:PID=1FC9:0083' in desc:
            try:
                ser = serial.Serial(
                    port=dev,
                    baudrate=1e6,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_TWO,
                    bytesize=serial.EIGHTBITS,
                    timeout=0.1
                )
                if not ser.isOpen():
                    raise Exception
                sers.append(ser)
            except:
                print 'Opening serial port {} failed.'.format(dev)
                raise
    if len(sers) < 2:
        raise Exception("Unable to find power detectors. Found {}.".format(len(sers)))

    freqs = np.linspace(100e6, 5.999e9, 600)
    try:
        real_freqs, samples = measure(sers, device, freqs)
        print np.mean(samples)

        with open('response.p', 'w') as f:
            pickle.dump((real_freqs, samples), f)
        plt.plot(real_freqs, samples)
        plt.show()
    finally:
        device.ctrl_transfer(0x40, 5, 0, (1 << 0) | (1 << 2) | (1 << 3)) #Signal, PA off
