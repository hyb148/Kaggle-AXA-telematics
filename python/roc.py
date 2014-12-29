import eau
import numpy

# sigmal distribution
a_s = 0.9
b_s = 1.0

# background distribution
a_b = 0.9
b_b = 1.0


# Sample sizes
n_s = 10000
n_b = 5000

# Generate signal and background samples
signal = (b_s - a_s) * numpy.random.random_sample(n_s) + a_s
background = (b_b - a_b) * numpy.random.random_sample(n_b) + a_b

# Scan the value from 0 to 1
TPR = numpy.zeros(100)
FPR = numpy.zeros(100)
for i in range(100):
    x = 0.01*i
    TPR[i] = sum( signal > x ) / n_s
    FPR[i] = sum( background > x ) / n_b

plotter = eau.Plotter()
plotter.scatterPlotXY( FPR, TPR )

aroc = eau.areaUnderLine( FPR, TPR )

print( "Area under ROC curve : " + str(aroc) )

input("PRESS ENTER")
