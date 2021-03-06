clc; clear; home;
close all;

disp("Starting...");

% Declare vars
readVarCount = 3;           % number of variables to be read from serial
sendVarCount = 3;           % number of variables to be sent by serial
readVarType = "string";     % datatype of variables read from serial
sendVarType = "double";     % datatype of variables sent by serial
BAUD = 115200;              % baud rate of serial connection
load('comPort.mat');        % loads COM port last used, ex: "COM5"  

% Get user input for COM port to use
portsAvailable = serialportlist("available");       % Lists the available COM ports
setCOMPort = 0;
if ~(isempty(portsAvailable))
    disp("Ports available:");
    disp(portsAvailable);
    while ~(setCOMPort)
        prompt = strcat("Please input COM port to use (press enter to use last used: ", comPort, "): ");
        inputStr = input(prompt, 's');
        if ~(inputStr == "")
            if (any(strcmp(portsAvailable, inputStr)))
                comPort = inputStr;
                disp(strcat("Using ", comPort, " port for serial connection"));
                setCOMPort = 1;
            else
                disp("Invalid input, try again...")
            end
        else
            disp(strcat("Using ", comPort, " port for serial connection"));
            setCOMPort = 1;
        end
    end
else
    disp("No COM ports available, please connect device(s)");
    return;
end


% Set serial communcation object
%comPortFile = matfile('comPort.mat', 'Writable', true);
save comPort.mat comPort -v7.3;         % Save comPort variable for use next time
teensy = serialport(comPort,BAUD);      % Create teensy object
configureTerminator(teensy, "CR/LF");   % Terminator set in teensy code
flush(teensy);                          % Flush serial bits
teensy.UserData = struct("time",[], ...
                         "var1",[], ...
                         "var2",[]);
sendData = struct("time",[], ...
                  "var1",[], ...
                  "var2",[]);

delayMS = 50;
hWaitbar = waitbar(0.99, 'connect', 'Name', 'Solving problem','CreateCancelBtn','delete(gcbf)');
for k=1:1e16
    % Read data from teensy via serial
    if (teensy.NumBytesAvailable > 0)
        start = tic;
        data = str2double(split(readline(teensy), ","));
        i = 1;
        teensy.UserData.time(end+1)     = data(i); i=i+1;
        teensy.UserData.var1(end+1)     = data(i); i=i+1;
        teensy.UserData.var2(end+1)     = data(i);
       
        % Display in data
        disp(data');
        
        % Modify data to send to teensy
        i = 1;
        sendData.time(end+1) = int8((toc(start))*1000);         i=i+1;
        sendData.var1(end+1) = teensy.UserData.var1(end) +1;    i=i+1;
        sendData.var2(end+1) = teensy.UserData.var2(end) +1;
        
        % Send data to teensy
        pause(delayMS/1000);         % Pause for n milliseconds
        vars = [sendData.time(end), sendData.var1(end), sendData.var2(end)];
        delim = ',';
        terminator = '?';
        sendString = append(strjoin(string(vars), delim), terminator);
        writeline(teensy, sendString);       
        
        % Display out data string
        disp(vars);
        disp("------------------------------");
    end
    if ~ishandle(hWaitbar)
        break;
    end
end
delete(hWaitbar);
delete(teensy);
disp("stopped connection");

