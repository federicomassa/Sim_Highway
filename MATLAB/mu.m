function mu(targetID)

sTargetID = sprintf('%02u', targetID);

a = dlmread(['Output/Out-Mu-T' sTargetID '.txt'], '\t');
aLength = size(a);
aLength = aLength(:,1);

ids = a(:, 3);
ids = unique(ids);
idsSize = size(ids);
idsSize = idsSize(1);

minNow = a(1, 1);
maxNow = max(a(:, 1));
cSteps = max(a(:, 2)) + 1;

xLength = (maxNow - minNow + 1) * cSteps;

mat = zeros(idsSize, xLength);

for i = 1:aLength
	for j = 1:idsSize
		if ids(j) == a(i, 3)
			mat(j, (a(i, 1) - minNow) * cSteps + a(i, 2) + 1) = a(i, 4);
		end;
	end;
end;

x = [0 : xLength - 1];

xLabels = zeros(xLength / cSteps + 1, 1);
ySize = size(xLabels);
ySize = ySize(1);
for i = 1:ySize
	xLabels(i) = i - 1;
end;

maxY = max(a(:, 4)) + 1;
minY = min(a(:, 4)) - 1;

for i = 1:idsSize
    subplot(2, 2, i);
	 stairs(x, mat(i, :));
    set(gca, 'XTick', 0:xLength);
    xlabel('Consensus steps');
    ylabel('Uncertainty (\mu)');
    set(gca,'XGrid','on');
    axis ([0, cSteps, minY, maxY]);
    title(['Agent ', num2str(ids(i))]);
end;
print('-deps2', ['Output/Out-Mu-T' sTargetID '-FirstSimulationStep.eps']);

for i = 1:idsSize
    subplot(2, 2, i);
	 stairs(x, mat(i, :));
    set(gca, 'XTick', 0:cSteps:xLength);
    set(gca, 'XTickLabel', xLabels);
    xlabel('Simulation steps');
    ylabel('Uncertainty (\mu)');
    set(gca,'XGrid','on');
    axis ([0, xLength - 1, minY, maxY]);
    title(['Agent ', num2str(ids(i))]);
end;
print('-deps2', ['Output/Out-Mu-T' sTargetID '.eps']);

end
