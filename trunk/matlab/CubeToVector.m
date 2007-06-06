function vec= CubeToVector(aCube,u,v)
        %roof
        if(aCube==1)
            vec=[(u-0.5)*2.0 1.0 -(v-0.5)*2.0];
        %left    
        elseif(aCube==2)
            vec=[-1.0 -(v-0.5)*2.0 -(u-0.5)*2.0];
        %front
        elseif(aCube==3)
            vec=[(u-0.5)*2.0 -(v-0.5)*2 -1.0];
        %right
        elseif(aCube==4)
            vec=[1.0 -(v-0.5)*2 (u-0.5)*2.0];
        %floor
        elseif(aCube==5)
            vec=[(u-0.5)*2.0 -1.0 (v-0.5)*2];
        %back
        elseif(aCube==6)
            vec=[(u-0.5)*2.0 (v-0.5)*2 1.0];
                 
    end
