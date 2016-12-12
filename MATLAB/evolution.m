function evolution(agentID)

sAgentID = sprintf('%02u', agentID);

a = dlmread(['Output/Out-Evolution-A' sAgentID '.txt'], '\t');
aLength = size(a);
aLength = aLength(:,1);

minNow = a(1, 1);
maxNow = max(a(:, 1));

x = [minNow : maxNow]';
xLength = (maxNow - minNow);

subplot(2, 2, 1);
plot(x, a(:, 2));
Min = min(a(:, 2));
Max = max(a(:, 2));
padding = (Max - Min) / 8;
%title(['Agent ' int2str(agentID) ' state evolution: x']);
title(['Agent state evolution: x']);
set(gca, 'XTick', 0:20:xLength);
set(gca,'XGrid','on');
set(gca,'YGrid','on');
xlabel('Simulation steps');
axis([0 xLength (Min - padding) (Max + padding)]);

subplot(2, 2, 2);
plot(x, a(:, 3));
%title(['Agent ' int2str(agentID) ' state evolution: y']);
title(['Agent state evolution: y']);
set(gca, 'XTick', 0:20:xLength);
set(gca,'XGrid','on');
set(gca, 'YTick', 0:1:3);
set(gca,'YGrid','on');
xlabel('Simulation steps');
axis([0 xLength 0 3])

subplot(2, 2, 3);
plot(x, a(:, 4));
Min = min(a(:, 4));
Max = max(a(:, 4));
padding = (Max - Min) / 8;
%title({' ';['Agent ' int2str(agentID) ' state evolution: \theta / \pi']});
title({' ';['Agent state evolution: \theta / \pi']});
set(gca, 'XTick', 0:20:xLength);
set(gca, 'YTick', -pi:pi/4:pi);
set(gca, 'YTickLabel', {'-1', '-0.75', '-0.5', '-0.25', '0', '0.25', '0.5', '0.75', 'pi'});
set(gca,'XGrid','on');
set(gca,'YGrid','on');
xlabel('Simulation steps');
axis([0 xLength (Min - padding) (Max + padding)]);

subplot(2, 2, 4);
plot(x, a(:, 5));
%title({' ';['Agent ' int2str(agentID) ' state evolution: v / v_{max}']});
title({' ';['Agent state evolution: v / v_{max}']});
set(gca, 'XTick', 0:20:xLength);
set(gca,'XGrid','on');
set(gca, 'YTick', 0:0.25:1);
set(gca,'YGrid','on');
axis([0 xLength 0 1]);
xlabel('Simulation steps');

print('-deps2', ['Output/Out-Evolution-A' sAgentID '.eps']);

end