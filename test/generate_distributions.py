#!/usr/bin/python
import numpy as np

def generate_distributions():
    # PARAMETERS
    cmin = 22330 # min value for uniform dist
    cmax = 71641 # max value for uniform dist
    cavg = 29338.9071 # mean for normal dist
    cstd = 3689.5235 # standard deviation for normal dist
    calp = 4.19843 # alpha for beta dist
    cbet = 24.6793 # beta for beta dist
    lam = 1.5 # lambda for exponential dist
    size = 100 # number of experiments 

    ###########
    # UNIFORM #
    ###########
    outFile = open("my_uniform.txt", "w")
    for i,item in enumerate(np.random.uniform(cmin,cmax,size)):                          
        outFile.write("%s  %s\n" % (i+1,item))                                  
    outFile.close()
    
    ########## 
    # NORMAL #
    ##########
    outFile = open("my_normal.txt", "w")
    for i,item in enumerate(np.random.normal(cavg,cstd,size)):
        outFile.write("%s  %s\n" % (i+1,item))
    outFile.close()
    
    ########
    # BETA #
    ########
    outFile = open("my_beta.txt", "w")
    for i,item in enumerate(np.random.beta(calp,cbet,size)):
        outFile.write("%s  %s\n" % (i+1,item))
    outFile.close()

    ###############
    # EXPONENTIAL #
    ###############
    outFile = open("my_exponential.txt", "w")
    for i,item in enumerate(np.random.exponential(lam,size)):
        outFile.write("%s  %s\n" % (i+1,item))
    outFile.close()

if __name__ == "__main__":
    generate_distributions()
