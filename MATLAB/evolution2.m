function evolution(agentID)

sAgentID = sprintf('%02u', agentID);

a = dlmread(['Output/Out-Evolution-A' sAgentID '.txt'], '\t');
aLength = size(a);
aLength = aLength(:,1);

plot(a(:, 2), a(:, 3));
Min = min(a(:, 3));
Max = max(a(:, 3));
pad = (Max - Min) / 8;
%title(['Agent ' int2str(agentID) ' state evolution: x, y']);
title(['Agent state evolution: x, y']);
set(gca,'XGrid','on');
set(gca,'YGrid','on');
axis([min(a(:, 2)) max(a(:, 2)) Min - pad Max + pad]);
ylabel('y');
xlabel({'x';' '});

print('-deps2', ['Output/Out-Evolution-A' sAgentID '_2.eps']);

end