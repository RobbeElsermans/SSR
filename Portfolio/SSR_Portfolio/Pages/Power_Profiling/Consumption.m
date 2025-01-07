start_time= 7.18; %in seconds
stop_time = 18; %in seconds
average_current = 0.011580; %im mA
time = stop_time-start_time
format longg
disp('total mAh');
mAh = (stop_time-start_time)/(60*60) * average_current
