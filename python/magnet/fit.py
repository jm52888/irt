import math
import traceback
from collections import defaultdict
from itertools import groupby, chain
import numpy as np, numpy.ma as ma
import scipy.optimize as spo
import scipy.stats as scistat
import statistics as stats
import bottleneck
import itertools
import magnet as mag

class position:
	def __init__(self):
		self.servo = 0
		self.slope = 0.0
		self.intercept = 0.0

def fit_lin_theilsen(x,y, sample= "auto", n_samples = 1e7):
    """
    Computes the Theil-Sen estimator for 2d data.
    parameters:
        x: 1-d np array, the control variate
        y: 1-d np.array, the ind variate.
        sample: if n>100, the performance can be worse, so we sample n_samples.
                Set to False to not sample.
        n_samples: how many points to sample.
        
    This complexity is O(n**2), which can be poor for large n. We will perform a sampling
    of data points to get an unbiased, but larger variance estimator.
    The sampling will be done by picking two points at random, and computing the slope,
    up to n_samples times.
    
    """
    
    def slope( x_1, x_2, y_1, y_2):
        if np.abs(x_2-x_1) == 0:
            return np.nan
        return (1 - 2*(x_1>x_2) )*( (y_2 - y_1)/np.abs((x_2-x_1)) )     

    assert x.shape[0] == y.shape[0], "x and y must be the same shape."
    n = x.shape[0]

    if n < 100 or not sample:
        ix = np.argsort( x )
        slopes = np.empty( n*(n-1)*0.5 )
        for c, pair in enumerate(itertools.combinations( range(n),2 ) ): #it creates range(n) =(
            i,j = ix[pair[0]], ix[pair[1]]
            slopes[c] = slope( x[i], x[j], y[i],y[j] )
    else:
        i1 = np.random.randint(0, n, n_samples)
        i2 = np.random.randint(0, n, n_samples)
        
        slopes = slope( x[i1], x[i2], y[i1], y[i2] )
        #pdb.set_trace()

    slope_ = bottleneck.nanmedian( slopes )
    #find the optimal b as the median of y_i - slope*x_i
    intercepts = np.empty( n )
    for c in range(n):
        intercepts[c] = y[c] - slope_*x[c]
    intercept_ = bottleneck.median( intercepts )

    return slope_, intercept_
    
#
# Fits slope & intercept using a linear regression.
#
def fit_lin_regress(speed, power):
    slope, intercept, r_val, p_val, stderr = scistat.linregress(speed, power)
    
    return slope, intercept

#
# Returns power as calculated from calibration (drag & speed_mps) with no
#
def magoff_power(speed_mps, drag, rr):
    return (( drag*(speed_mps**2) + (rr) ) * speed_mps )    

#
# Returns the magnet only portion of power by subtracting estimated power at 
# speed given drag and rolling resistance.
#
def magonly_power(speed_mps, power, drag, rr):
    magoff = magoff_power(speed_mps, drag, rr)
    magonly_power = power - magoff

    # must by positive
    if magonly_power < 0:
        magonly_power = 0    
        
    return magonly_power
    
#
# x = speed_mps
# y = power
#
def fit_bike_science(x, y):
    pars, covar = spo.curve_fit(magoff_power, x, y)
    #print('bike', pars)
    #plt.plot(x_new, bike_science_func(x_new1, *pars), 'b+', zorder=100, linewidth=3)
    #labels.append(r'%s' % ('Bike Function'))        
    
    return pars[0], pars[1]
    
#
# Fits calibration data for all records at servo position 2000.
#
def fit_nonlinear_calibration(records):
    """
    Cluster speeds and find the median watts for these speeds.
    """
    groups = []

    #TODO: need to take groups of nearby speeds... not this.
    
    keyfunc = lambda x: float(x['speed'])*0.44704
    data = sorted(records, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = []
        for i in g:
            items.append(i[1])
        med = np.mean(items) #TODO: change this to mean/average?
        groups.append((k, med))

    npgroups = np.array(groups)
    x = npgroups[:,0]
    y = npgroups[:,1]

    #x = [10.1* 0.44704, 15.1* 0.44704, 20.1* 0.44704, 25.2* 0.44704]
    #y = [74, 113, 158, 217]
    
    return fit_bike_science(x, y)
        
def fit_polynomial(x, y, color):
	x_new = np.arange(800, 1600, 100)
	coefficients = np.polyfit(x, y, 3)
	polynomial = np.poly1d(coefficients)
	ys = polynomial(x_new)
	# y = ax^2 + bx + c
	f = ("y = %sx^3 + %sx^2 + %sx + %s" % (coefficients[0], coefficients[1], coefficients[2], coefficients[3]))
	#print(r)
	#plt.subplot(2, 1, 1)
	#plt.plot(x_new, ys, linestyle='--', color=color)

	# return the text
	return f, coefficients

def interop_poly(coeffs, mps):
	# takes 2 polynomials and linearly interopolates by speed.
	return 0
	
def get_power(coeff, mps, servo_pos):
	f = np.poly1d(coeff)
	y = f(servo_pos)
		
def fit_3rd_poly(positions):
	values = []
	polys = []	
	colors = iter(['r','g','b','y','c'])
	labels = []
	#plt.subplot(2, 1, 1)
	#plt.grid(b=True, which='major', color='gray', linestyle='--')
	#plt.ylabel('Watts @ Position by Speed')
	# generate speed data 5-25 mph
	# calculate power for each position
	for mph in range(10,26,5):
		c = next(colors)
		
		mps = mph*0.44704
		power = []
		servo_pos = []

		for p in positions:
			power.append(mps * p.slope + p.intercept)
			servo_pos.append(p.servo)
			#print(p.servo, mph, watts)
			
		#plt.plot(servo_pos, power, color=c)
		labels.append(r'%1.1f' % (mph))
		
		f, coeff = fit_polynomial(servo_pos, power, c)
		values.append((mph, coeff))

		#print(mph, f)
		
		#print(p.intercept)
		
	#plt.legend(labels, loc='upper right')
	return values
		
def fit_linear(positions):
	#plt.subplot(2, 1, 2)
	#plt.ylabel('Watts @ Speed by Servo Position')
	#plt.grid(b=True, which='major', color='gray', linestyle='--')
	
	for p in positions:
		speed = []
		power = []
		
		for mph in range(5,25,5):
			speed.append(mph)
			power.append((mph * 0.44740) * p.slope + p.intercept)
			#plt.plot(speed, power)        

def get_position_data():
	# loads slope & intercept for each position.
	positions = []
	data = np.loadtxt('position_slope_intercept.csv', delimiter=',', comments='#')
	
	for r in data:
		p = position()
		p.servo = r[0]
		p.slope = r[1]
		p.intercept = r[2]
		positions.append(p)

	return positions
            
def init_mag():
    positions = get_position_data()
    values = fit_3rd_poly(positions)
    
    low_speed = values[1][0]*0.44704
    low_a = values[1][1][0]
    low_b = values[1][1][1]
    low_c = values[1][1][2]
    low_d = values[1][1][3]

    high_speed = values[3][0]*0.44704
    high_a = values[3][1][0]
    high_b = values[3][1][1]
    high_c = values[3][1][2]
    high_d = values[3][1][3]
    
    mag.set_coeff(low_speed, 
        low_a, 
        low_b, 
        low_c, 
        low_d, 
        high_speed, 
        high_a, 
        high_b, 
        high_c,
        high_d)
        
    return mag

#
# Calculates power based coast down fit (drag & rr), speed and magnet position.
#
def power_estimate(speed_mps, servo_pos, drag, rr):
    # Calculates non-mag power based on speed in meters per second.
    no_mag_watts = magoff_power(speed_mps, drag, rr)
    
    # If the magnet is ON the position will be less than 1600
    if servo_pos < 1600:
        # Calculates the magnet-only power based on speed in meters per second.
        mag_watts = mag.watts(speed_mps, servo_pos)
        #print(servo_pos, mag_watts)
    else:
        mag_watts = 0	
    
    return no_mag_watts + mag_watts          
    
    
#
# Calculates a moving average of an array of values.
#
def moving_average(x, n, type='simple'):
    """
    compute an n period moving average.

    type is 'simple' | 'exponential'

    """
    x = np.asarray(x)
    
    if type=='simple':
        weights = np.ones(n)
    else:
        weights = np.exp(np.linspace(-1., 0., n))

    weights /= weights.sum()


    a =  np.convolve(x, weights, mode='full')[:len(x)]
    a[:n] = a[n]
    return a
    
#
# Calculates a moving average of speed which eliminates outliers where
# we may have had data drop.
#
def speed_moving_average(speed, n):
    stdev_speed = stats.stdev(speed)
    
    # look backward and forward to determine if the speed is an
    # outlier or it's a legitimate change in speed.
    def normalize_speed(speed):
        for ix in range(0, len(speed), 1):
            if ix < 1 or ix+1 > len(speed):
                yield speed[ix]
            else:
                if abs(speed[ix-1] - speed[ix]) > stdev_speed and \
                        abs(speed[ix+1] - speed[ix]) > stdev_speed:
                    # Return previous speed.
                    yield speed[ix-1]
                else:
                    yield speed[ix]
    
    normal_speed = normalize_speed(speed)
    ma = moving_average(list(normal_speed), n)
    
    return ma

#
# Returns the index into the power array where the longer moving average
# crosses the shorter moving average.
# 
# Returns at iterator which yields: index, power moving average, speed
#
def power_ma_crossovers(records):
    # don't take any data points within # of seconds of a servo change.
    servo_lag = 6                  
    speed_sec = 15
    long_power_sec = 15
    short_power_sec = 5

    power = records['power']
    
    ma_speed = speed_moving_average(records['speed'], speed_sec)
    ma_long = moving_average(power, long_power_sec)
    ma_short = moving_average(power, short_power_sec) 
    
    for i in range(1, len(power)-2, 1):
        # if the last long average was less than short average
        # and the current long average is higher than short
        # we've crossed over.
        if ma_long[i-1] < ma_short[i-1] and ma_long[i] > ma_short[i]:
            # Only include if the servo position hasn't changed for a few seconds.
            # This eliminates the issue with averages appearing on the edge.
            if i > servo_lag and records['position'][i-servo_lag] == records['position'][i]:
                # We've crossed over return index, position, power moving average, speed.
                yield i, ma_long[i], ma_speed[i]
    