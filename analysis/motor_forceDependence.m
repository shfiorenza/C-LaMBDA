%{
clear variables;
sim_name = 'run_motor_forceVel/k401_forceVel';
applied_forces = [-6:0.25:0];
seeds = [0 , 1, 2, 3];

exp_forces = [0, -1.0 : -0.5 : -6.0];
exp_runlengths = [940, 770, 480, 500, 270, 330, 250, 150, 240, 120, 70, 50];
exp_err_runlengths = [130, 80, 40, 60, 20, 30, 20, 10, 10, 9, 9, 10];
exp_velocities = [750, 690, 720, 640, 630, 540, 440, 390, 250, 230, 140, 120];
exp_err_velocities = [6, 5, 5, 6, 10, 6, 5, 6, 5, 6, 5, 5];

file_dir = '/home/shane/projects/CyLaKS';

n_runs = length(applied_forces);
avg_runlengths = zeros(n_runs, 1);
err_runlengths = zeros(n_runs, 1);
avg_lifetimes = zeros(n_runs, 1);
err_lifetimes = zeros(n_runs, 1);
avg_velocities = zeros(n_runs, 1);
err_velocities = zeros(n_runs, 1);
for i_run = 1 : n_runs 
    name = sprintf("%s/%s_%g", file_dir, sim_name, abs(applied_forces(i_run)));
    mot_stats = get_motor_stats(name, seeds);
    avg_runlengths(i_run) = mot_stats(1);
    err_runlengths(i_run) = mot_stats(2);
    avg_lifetimes(i_run) = mot_stats(3);
    err_lifetimes(i_run) = mot_stats(4);
    avg_velocities(i_run) = mot_stats(5);
    err_velocities(i_run) = mot_stats(6);
end
%}

% Run length plot %
fig1 = figure();
set(fig1, 'Position', [50, 50, 1.5*480, 1.5*360]);
hold all
% Plot sim data
sim_run = errorbar(applied_forces, avg_runlengths, err_runlengths, 'o', ...
    'MarkerSize', 10, 'LineWidth', 2);
sim_run.MarkerFaceColor = sim_run.MarkerEdgeColor;
% Plot experimental data
exp_run = errorbar(exp_forces, exp_runlengths, exp_err_runlengths, '^', ...
    'MarkerSize',14, 'LineWidth', 2, 'Color', 	[0.8500, 0.3250, 0.0980]);
exp_run.MarkerFaceColor = exp_run.MarkerEdgeColor;
% Plot experimental fit 
L_0 = 1120;         % in nm
sigma_off = 2.0;    % in nm
kbT = 4.114;        % in pN * nm
exp_run_fit = fplot(@(f) L_0*exp(-abs(f) * sigma_off/kbT), [-6 0], ...
    'LineWidth', 3, 'Color', [0.8500, 0.3250, 0.0980]);
% Bring sim data to front
uistack(sim_run, 'top');

% Label axes, legend, etc. 
xlabel('Applied force (pN)');
ylabel('Run length (nm)');
set(gca, 'FontSize', 20);
xlim([-6.5 0.5]);
ylim([0 1400]);
yticks([0 250 500 750 1000 1250]);
legend({'Experiment - Andreasson et al.', 'Model - Andreasson et al.', 'Simulation'}, ... 
    'location', 'northwest', 'FontSize', 14);

fig2 = figure();
set(fig2, 'Position', [75, 75, 1.5*480, 1.5*360]);
hold all
sim_vel = errorbar(applied_forces, avg_velocities, err_velocities, 'o', ...
    'MarkerSize', 10, 'LineWidth', 2);
sim_vel.MarkerFaceColor = sim_vel.MarkerEdgeColor;
% Plot experimental velocity data
exp_vel = errorbar(exp_forces, exp_velocities, exp_err_velocities, '^', ...
    'MarkerSize', 14, 'LineWidth', 2, 'Color', 	[0.8500, 0.3250, 0.0980]);
exp_vel.MarkerFaceColor = exp_vel.MarkerEdgeColor;
% Plot experimental velocity fit
d_step = 8.2;         % in nm
F_i = 26;           % in pN
k0_1 = 5000;         % in 1/s
sigma_1 = 4.6;      % in nm
k0_2 = 95;          % in 1/s
k0_3 = 260;          % in 1/s
sigma_3 = 0.35;     % in nm
k_1 = @(f) k0_1 * exp(f * sigma_1 / kbT);
k_2 = k0_2; 
k_3 = @(f) k0_3 * exp((f + F_i) * sigma_3 / kbT);
exp_vel_fit = fplot(@(f) d_step*k_1(f)*k_2*k_3(f) / (k_1(f)*k_2 + k_3(f)*(k_1(f) + k_2)), ...
    [-6 0], 'LineWidth', 3, 'Color', [0.8500, 0.3250, 0.0980]);

uistack(sim_vel, 'top');
xlabel('Applied force (pN)');
ylabel('Velocity (nm/s)');
set(gca, 'FontSize', 20);
xlim([-6.5 0.5]);
ylim([0 900]);
legend({'Experiment - Andreasson et al.', 'Model - Andreasson et al.', 'Simulation'}, ... 
    'location', 'northwest', 'FontSize', 14);
