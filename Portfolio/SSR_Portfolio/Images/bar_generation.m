% create bar charts

x = ["Delay", "Stop 2", "Standby"];
vals = [ 9 9 9.24 ;  
    9.1 0.0243 4.3;
    9.08 0.00225 4.35];
b = bar(x,vals);
xtips1 = b(1).XEndPoints;
ytips1 = b(1).YEndPoints;
labels1 = string(b(1).YData);
text(xtips1,ytips1,labels1,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

xtips2 = b(2).XEndPoints;
ytips2 = b(2).YEndPoints;
labels2 = string(b(2).YData);
text(xtips2,ytips2,labels2,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

xtips3 = b(3).XEndPoints;
ytips3 = b(3).YEndPoints;
labels3 = string(b(3).YData);
text(xtips3,ytips3,labels3,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

legend('run', 'wait', 'total');

ylabel("Current Consumption (mA)");
grid off;

%% nRF52
x = ["normal","normal modified", "deep sleep"];
vals = [ 0.5 6.22 4.42 ;  
     0.5 0.004 0.27;
    0.5 0.002 0.28];
b = bar(x,vals);
xtips1 = b(1).XEndPoints;
ytips1 = b(1).YEndPoints;
labels1 = string(b(1).YData);
text(xtips1,ytips1,labels1,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

xtips2 = b(2).XEndPoints;
ytips2 = b(2).YEndPoints;
labels2 = string(b(2).YData);
text(xtips2,ytips2,labels2,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

xtips3 = b(3).XEndPoints;
ytips3 = b(3).YEndPoints;
labels3 = string(b(3).YData);
text(xtips3,ytips3,labels3,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

legend('blink', 'wait', 'total');

ylabel("Current Consumption (mA)");
grid off;

%% Sensors
x = ["SHT40","LTR-329", "deep sleep"];
vals = [ 0.5 6.22 4.42 ;  
     0.5 0.179 0.27;
    0.5 0.177 0.28];
b = bar(x,vals);
xtips1 = b(1).XEndPoints;
ytips1 = b(1).YEndPoints;
labels1 = string(b(1).YData);
text(xtips1,ytips1,labels1,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

xtips2 = b(2).XEndPoints;
ytips2 = b(2).YEndPoints;
labels2 = string(b(2).YData);
text(xtips2,ytips2,labels2,'HorizontalAlignment','center',...
    'VerticalAlignment','bottom')

legend('blink', 'wait', 'total');

ylabel("Current Consumption (mA)");
grid off;