start_time= 0; %in seconds
stop_time = 25; %in seconds
average_current = 1.01; %im mA
time = stop_time-start_time
format longg
disp('total mAh');
mAh = (stop_time-start_time)/(60*60) * average_current
