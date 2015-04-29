#!/usr/bin/python
import numpy as np

def normalize(li):
    _sum = sum(li)
    return np.where((li>0),li/_sum,li)

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
    li = normalize(np.random.uniform(cmin,cmax,size))
    for i,item in enumerate(li):                          
        outFile.write("%s  %s\n" % (i+1,item))                                  
    outFile.close()
    
    ########## 
    # NORMAL #
    ##########
    outFile = open("my_normal.txt", "w")
    li = normalize(np.random.normal(cavg,cstd,size))
    for i,item in enumerate(li):
        outFile.write("%s  %s\n" % (i+1,item))
    outFile.close()
    
    ########
    # BETA #
    ########
    outFile = open("my_beta.txt", "w")
    li = normalize(np.random.beta(calp,cbet,size))
    for i,item in enumerate(li):
        outFile.write("%s  %s\n" % (i+1,item))
    outFile.close()

    ###############
    # EXPONENTIAL #
    ###############
    outFile = open("my_exponential.txt", "w")
    li = normalize(np.random.exponential(lam,size))
    for i,item in enumerate(li):
        outFile.write("%s  %s\n" % (i+1,item))
    outFile.close()

if __name__ == "__main__":
    generate_distributions()
