#ifndef _CMD_H
#define _CMD_H

#include "../utils/config.h"
#include <bits/stdc++.h>
#include "../renderer/rendererptls.h"

class Cmd
{
public:
    void main(const std::vector<std::string>& args)
    {
        Config config;

        // Cmds are seped by spaces
        // Format: <scene desc> --output <output filename> --params <render setting>
        // Example: nuRay.exe  pot.obj -s 10 cube.obj -s 10 -p 0 -10 0  --output result.jpg  --params renderer=ptnee imgw=128 imgh=128 spp=16 campos=0,2,0 cameuler=0,0,0
        // DO NOT add any space in one kvpair

        std::string scene_desc;
        std::string output_filename;
        
        int stage = 0;
        for(auto &s: args)
        {
            if(s=="--output")
            {
                stage=1;
            }
            else if(s=="--params")
            {
                stage=2;
            }
            if(stage==0)
            {
                scene_desc+=s;
                scene_desc+=" ";
            }
            else if(stage==1)
            {
                if(s!="--output")
                {
                    output_filename=s;
                }
            }
            else if(stage==2)
            {
                config.addItem(s);
            }
        }

        std::cout << "config list" << std::endl;
        config.print();

        std::string renderer_name = config.getValueStr("renderer", "ptnee");
        RendererPTLS r;
        r.render()
    }
};

#endif