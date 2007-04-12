function c=reconstructionstep(c)
limit=size(c,2)/2;
temp=zeros(size(c));
for i=1:limit
    temp(1,2*i-1,1)=(c(1,i,1)+c(1,limit+i,1))/sqrt(2);
    temp(1,2*i,1)  =(c(1,i,1)-c(1,limit+i,1))/sqrt(2);

%     temp(1,2*i-1,1)=(c(1,i,1)+c(1,limit+i,1))/sqrt(2);
%     temp(1,2*i,1)  =(c(1,i,1)-c(1,limit+i,1))/sqrt(2);
% 
%     temp(1,2*i-1,2)=(c(1,i,2)+c(1,limit+i,2))/sqrt(2);
%     temp(1,2*i,2)  =(c(1,i,2)-c(1,limit+i,2))/sqrt(2);
% 
%     temp(1,2*i-1,3)=(c(1,i,3)+c(1,limit+i,3))/sqrt(2);
%     temp(1,2*i,3)  =(c(1,i,3)-c(1,limit+i,3))/sqrt(2);    

end

c=temp;
