clear all
n_timesteps = 100000;
length_of_microtubule = 1000;
occupancy_threshold = 0.4;  %threshold below which the avg
                            %occupancy is considered mutant

temp_one = zeros([length_of_microtubule 1]);
final_mt = zeros([length_of_microtubule 1]);

polarityArray = {'Plus-end on right'};

fileDirectory = '/home/shane/Projects/overlap_analysis/new_mt_single/%s';
fileName = 'test.file';

data_file = fopen(sprintf(fileDirectory, fileName));
raw_data = fread(data_file, [length_of_microtubule, n_timesteps], '*int');
fclose(data_file);

raw_data((raw_data ~= 2) | (raw_data == 0)) = 0;
raw_data((raw_data == 2) | (raw_data ~= 0)) = 1;

for i=1:1:n_timesteps
    temp_one(:, 1) = raw_data(:, i);
    final_mt(:, 1) = final_mt(:, 1) + double(temp_one(:, 1)./n_timesteps);
end
n_normal = 0;
n_mutant = 0;
for j=1:1:length_of_microtubule
   if final_mt(j, 1) > occupancy_threshold
       n_normal = n_normal + 1;
   else
       n_mutant = n_mutant + 1;
   end
end
%%plot fig%%
fig1 = figure(1);
set(fig1,'Position', [50, 50, 2.5*480, 2.5*300])
plot(linspace(0, 1, length_of_microtubule), final_mt);

%%style stuff%%
grid on
grid minor
xlabel({'Fractional length of microtubule', sprintf('(N Sites = %d; %d normal, %d mutant)', length_of_microtubule, n_normal, n_mutant)});
ylabel('Fraction of the time occupied');
axis = gca;
axis.YLim = [0 1];
axis.XLim = [0 1];
axis.TickDir = 'out';
axis.Box = 'off';
axis.GridLineStyle = '-';
set(findall(axis, 'Type', 'Line'), 'LineWidth', 0.5);
legend(polarityArray, 'Location', 'northeastoutside');
