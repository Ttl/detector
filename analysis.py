import sys
import numpy as np
import pickle
import matplotlib.pyplot as plt

f, x_short = pickle.load( open('short.p', 'r'))
_, x_open = pickle.load( open('open.p', 'r'))

tau = np.array([(x_short[i] + x_open[i])/2 for i in xrange(len(x_short))])
f = [i/1e9 for i in f]

for y in sys.argv[1:]:
    _, x_response = pickle.load( open(y, 'r'))

    name = y[:y.find('.')]
    plt.plot(f, x_response-tau, label=name)
    plt.ylabel('Return loss [dB]')
    plt.xlabel('Frequency [GHz]')
plt.legend(loc='lower left')
plt.show()
