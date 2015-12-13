MapFile   = 'BrandeisMapLabeledCropped.jpg';
RouteFile = 'RouteCropped.txt';

%Display the map:
Map = imread(MapFile);
imshow(Map,'Border','tight');
hold on

%Load the path data:
P = load(RouteFile);

%Plot the route using color magenta:
x = P(:,1); 
y = P(:,2);
u = P(:,3)-x;
v = P(:,4)-y;
quiver(x,y,u,v,0,'color','m','linewidth',3,'autoscale','off','maxheadsize',.03);
