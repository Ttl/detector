import sys
import serial
import serial.tools.list_ports
from scipy.interpolate import interp1d
from ad8319cal import cal_table

for k in cal_table.keys():
    cal_table[k] = zip(*cal_table[k])

def v_to_dbm(v, freq):
    freq /= 1e6
    cal_freqs = sorted(cal_table.keys())
    x = []
    for f in cal_freqs:
        fx, fy = cal_table[f]
        x.append(interp1d(fy, fx, fill_value='extrapolate')(v))
    return interp1d(cal_freqs, x, fill_value='extrapolate')(freq) + 3.3 #From parallel 50 ohm termination

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Give frequency in GHz as argument"
        exit()
    freq = float(sys.argv[1])*1e9
    if freq < 0 or freq > 10e9:
        print "Frequency out of range 0 < freq < 10"
        exit()
    ports = serial.tools.list_ports.comports()
    ser = None
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
            except:
                print 'Opening serial port {} failed.'.format(dev)
                raise
    if ser == None:
        raise Exception("Unable to find device")

    #Set T_ADJ
    if freq >= 5.3e9:
        #500 ohm if f > 5.3 GHz
        ser.write('\xf1')
    else:
        #8.2k if f < 5.3 GHz
        ser.write('\xf0')

    while True:
        try:
            x = ord(ser.read(1))
            if x != 0xFF:
                continue
            x = map(ord,ser.read(2))
            if x[0] & 0xF0 != 0x00:
                #Not synchronized
                continue
            y = ((x[0] & 0x0F) << 8) | x[1]
            print v_to_dbm(3.3*y/4095.0, freq)
        except serial.serialutil.SerialException:
            continue
        except OSError:
            continue
        except KeyboardInterrupt:
            print "Exiting"
            break
