close all;
clear all;
clc;

% Type of distribution ({'U', 'N', 'B'})
distribution = 'N';

% Minimum and maximum computation time
cmin = 22330;
cmax = 71641;
cavg = 29338.9071;
cstd = 3689.5235;
calp = 4.19843;
cbet = 24.6793;

% Total number of experiments
max_numbers = 100000;

% Total number of bins in the pmf
num_bins = 1000;

% Random generation of the computation time
save_pmf = 0;
if strcmp(distribution, 'U')
    %fileName = 'uniform.txt';
    compTime = cmin + ((cmax - cmin) * rand(max_numbers, 1));
    %save(fileName, 'compTime', '-ascii')
	fileName = ['uniform-' num2str(num_bins) '.txt'];
elseif strcmp(distribution, 'N')
	%fileName = 'normal.txt';
    compTime = normrnd(cavg, cstd, [max_numbers 1]);
    %save(fileName, 'compTime', '-ascii')
    fileName = ['normal-' num2str(num_bins) '.txt'];
elseif strcmp(distribution, 'B')
	%fileName = 'beta.txt';
    compTime = betarnd(calp, cbet, [max_numbers 1]);
    compTime = compTime * 100000;
    %save(fileName, 'compTime', '-ascii')
    fileName = ['beta-' num2str(num_bins) '.txt']; 
end

% Generate the histogram
[num_elements, centers] = hist(compTime, num_bins);
 
% Normalize the histogram
num_elements = num_elements / sum(num_elements);

% Save the pmf for the analytical solution
if save_pmf
    T = [centers; num_elements];
    fid = fopen(fileName, 'w');
	fprintf(fid,'%6.2f  %12.8f\n', T);
end

% Plot the normalized histogram
figure(1); clf;
bar(centers, num_elements, 'histc');

% Graphic information
title('Normalized Histogram of the Computation Time');
xlabel('Computation Time [us]')
ylabel('Probability')

% CDF
figure(2); clf;

% Obtain the Synthetic CDF
cdfplot(compTime);

% Graphic information
title('Cumulative Distribution')
xlabel('Computation Time [us]')
ylabel('Probability')
