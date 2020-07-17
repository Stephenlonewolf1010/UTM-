%'winvideo', 1, 'MJPG_1280x720' (laptop'cam retireved from Image aquistion)
%'winvideo', 2, 'Y800_2456x2052' (external cam name from Imaq app)

%% serial port arduino opening
delete(instrfindall)
serialportlist('available')
s = serial('COM4')
s.BaudRate =115200;
fopen(s);
fprintf(s, '%s', 'M');
% guih = getView(s);
%%  testing IM aq toolbox
hasIPT = license('test', 'image_toolbox');
if ~hasIPT
	% User does not have the toolbox installed.
	message = sprintf('Sorry, but you do not seem to have the Image Processing Toolbox.\nDo you want to try to continue anyway?');
	reply = questdlg(message, 'Toolbox missing', 'Yes', 'No', 'Yes');
	if strcmpi(reply, 'No')
		% User said No, so exit.
		return;
	end
end
%% 
vid = videoinput('winvideo', 2, 'Y800_2456x2052'); %(creating vid inputobj(adaptorname, deviceID,vidformat) ...
                                                %if error comes while detecting, go to apps/image aquisition/tools/refresh)

src.Brightness = 0;
src.Exposure = 0;
src.Gain = 0;
src.FrameRate = '60.0002'
vid.ROIPosition = [0 0 2456 200];
triggerconfig(vid, 'manual');
%vid.FramesPerTrigger  = inf'; %camera framerate persec x time for viewing
vid.FrameGrabInterval = 2 %time taken to log in the data
% vid.ReturnedColorspace = 'grayscale';
vid_src = getselectedsource(vid);


pointTracker = vision.PointTracker('MaxBidirectionalError', 2)
blobAnalysis = vision.BlobAnalysis('AreaOutputPort', false, ... % Set blob analysis handling
    'CentroidOutputPort', true, ...
    'BoundingBoxOutputPort', true', ...
    'MinimumBlobArea', 100, ...
    'MaximumBlobArea', 1000, ...
    'MaximumCount', 8);


vid_src.Tag = 'motion detection setup';


% thresholdblack=0;


set(gcf,'doublebuffer','on');
preview(vid);

start(vid)
trigger(vid);
vid.LoggingMode = 'memory';

nframe = 0
timeforexp=1; %just to run an inf loop
%checkbox to hit stop

hCheckbox = uicontrol('Style','checkbox',...
    'Units', 'Normalized',...
    'String', 'Finish Now',...
    'Value',0,'Position', [.8 .100 .4 .05], ...
    'FontSize', 14);

%% material sample properties 
Area = 13*3;  %write area of sample for stress F/A in mm^2;
Dis = 1024;
color = 0; % just a condition to work on black or white test piece,change value to any other +integer to work on black 0 for white
Fmax =0; 
n=0;
caliberation_factor  =1; 
% % %% serial port arduino opening
% % delete(instrfindall)
% % serialportlist('available')
% % s = serial('COM4')
% % s.BaudRate =115200;
% % fopen(s);
%% plotting 
subplot(2,2,1); 
h.plot = plot(NaN,NaN,'-k'); hold on;
h.meanPlot = plot(NaN,NaN,'-b');
data.X = [];
data.Y = [];
data.Xm = [];
data.Ym = [];
xlabel('strain')
ylabel('stress')
fprintf(s,'%s','M')
%% loop
while timeforexp ==1
% for  n=1:500
    % read the next video frame
    frame = getsnapshot(vid);
    diff_im =  imsubtract(frame(:,:,1), rgb2gray(frame));%
    newframe = im2bw(frame);
    if color == 0
        newframe1 = imcomplement(newframe);
    else
        newframe1=newframe;
    end
    [centroid, bbox] = blobAnalysis(newframe1);
    centroid = double(centroid); % Convert the centroids into Integer for further steps
    
    frame = insertShape(frame,'Rectangle', bbox, 'Color', 'green');
    subplot(2,1,2),imshow(frame);
    numblobs = size(bbox, 1);
    
    if numblobs>=1
        axis on;
        hold on;
        caption = sprintf('%d blobs found in frame', numblobs);
        title(caption, 'FontSize', 10);
        for i = 1 :numblobs
            cX = centroid(i,1); %x-coordinates of the points
            cY =centroid(i,2); %y-coordinates  of points
            hplot= plot(cX,cY,'y+' ,'MarkerSize', 5, 'LineWidth', 2);
            hText = text(cX, cY,strcat('X:',num2str(round(cX)),'Y:',num2str(round(cY))));
            for j =1
                %Y(i,j) = [round(cY)]
                X(i,j) =[round(cX)];
            end
        end
        D = X(1,:) -X(end,:); %finds the toppest point and last point in the image and subtracts to fins the distance
        distance = (D/Dis);
        hold off
%         drawnow; (not rquired now coz its already used for updating plot
%         in the next step
        
    else
        msgbox('dang!, no blobs found'); %(delete this else loop if it anoys you)
        pause(0.5);
%        break;
    end
  strain = abs(distance) * caliberation_factor     ;      %abs for only +values
 out = fscanf(s);
F = round(abs(str2num(out)))

if F >= Fmax
    Fmax = F 
end
if F < Fmax *0.6 && F>0
    msgbox('bye');
  
     txtInfo = sprintf('E = %.1f %s  Mpa', E);
	msgbox(txtInfo);
     return;
%     break;
end
  stress = F *9.8/ Area;
  
data.X(end+1) = strain;
data.Y(end+1) = stress;

% set(h.plot, 'xdata', data.X, 'ydata', data.Y);
if length(data.X) > 50
   data.Xm(end+1) = mean(data.X(end-50:end));
   data.Ym(end+1) = mean(data.Y(end-50:end));
   set(h.meanPlot, 'XData', data.Xm, 'Ydata', data.Ym); 
end
drawnow;
	
maxstress = Fmax*9.8/Area; 
   E = maxstress/strain ;
    if get(hCheckbox, 'Value')      % a check button to finish the task
        % Finish now checkbox is checked.
    txtInfo = sprintf('E = %.1f %s  Mpa', E);
	msgbox(txtInfo);
    	fprintf(1, '%\n', txtInfo);

        msgbox('close all tabs , Thankyou ');
        return;
    end

end
	
	% Print the values out to the command window.

stop(vid)
delete(vid)
fclose(s);
delete(s)
clear s

clear videoReader
close(gcf)

