close all;

% A code to generate the power profiling bar charts
x = [1 2 3];
vals = [2 11 ; 3 6 ; 23 26];
b = bar(x,vals);
b(1).Labels = b(1).YData;
b(2).Labels = b(2).YData;

figure(2);
Y=[198 138 172 188 190 192];
bar(Y);
text(1:length(Y),Y,num2str(Y'),'vert','bottom','horiz','center');
box off
