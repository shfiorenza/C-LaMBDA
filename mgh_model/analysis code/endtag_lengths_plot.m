clear all

mt_lengths = [2, 4, 6, 8, 10, 14];

n_datapoints = length(mt_lengths);

final_data_short = [n_datapoints 1];
final_data_short(1, :) = 1.16;
final_data_short(2, :) = 1.752;
final_data_short(3, :) = 2.056;
final_data_short(4, :) = 2.12;
final_data_short(5, :) = 2.024;
final_data_short(6, :) = 2.075;

final_data_long = [n_datapoints 1];

final_data_long(1, :) = 1.4;
final_data_long(2, :) = 1.824;
final_data_long(3, :) = 1.944;
final_data_long(4, :) = 1.992;
final_data_long(5, :) = 1.976;
final_data_long(6, :) = 1.928;

plot(mt_lengths, final_data_short);
hold on
%plot(linspace(2, 14, 6), final_data_long);

xlabel('Length of microtubule (microns)');
ylabel('Length of endtag (microns)');