function vec= CubeToVector(aCube,u,v)
        if(aCube==1)
            vec=[(u-0.5)*2.0 1.0 -(v-0.5)*2.0];
        elseif(aCube==2)
            vec=[-1.0 -(v-0.5)*2.0 -(u-0.5)*2.0];
        elseif(aCube==3)
            vec=[(u-0.5)*2.0 -(v-0.5)*2 -1.0];
        elseif(aCube==4)
            vec=[1.0 -(v-0.5)*2 (u-0.5)*2.0];
        elseif(aCube==5)
            vec=[(u-0.5)*2.0 -1.0 (v-0.5)*2];
        elseif(aCube==6)
            vec=[(u-0.5)*2.0 (v-0.5)*2 1.0];
                 
    end