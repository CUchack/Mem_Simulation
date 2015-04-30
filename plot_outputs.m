%This script is to process data from the cache simulation and create 
%pretty graphs for the data.
%
%Data to graph:
%Average CPI per configuration
%Hit rates per configuration (R/W/I/Avg)
%



files = dir(fullfile('output*'));
%files = char({files.name});

for i = 1:numel(files)
%    [filename,pathname] = getfile(files(i,:));
%    file = files(i,:);
    fid = fopen(files(i).name);
    fline = fgetl(fid);
%    start = regexp
    j = 0;
    while ischar(fline)
        s = strfind(fline,'Simulation');
        if s
            endline = strfind(fline,char(10));
            tracename = fullfile(fline(s+23:endline));
%        else if fline (
        end
        fline = fgetl(fid);
    end
    fclose(fid);
end

            
    