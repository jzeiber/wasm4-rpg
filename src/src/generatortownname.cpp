#include "generatortownname.h"
#include "randommt.h"
#include "printf.h"

void GenerateTownName(const uint64_t seed, char *name, const int16_t len)
{
    RandomMT &r=RandomMT::Instance();
    int16_t pos=0;
    int16_t lenleft=len;
    const int8_t prefixcount=2;
    const int8_t directioncount=4;
    const int8_t basecount=23;
    const int8_t suffixcount=11;
    static char *prefix[]={{"Olde"},{"New"}};
    static char *direction[]={{"North"},{"East"},{"South"},{"West"}};
    static char *base[]={{"Bogo"},{"Breeze"},{"Bridge"},{"Bright"},{"Eagle"},{"Ever"},{"Feather"},{"Flower"},{"Fox"},{"Haver"},{"Living"},{"Moss"},{"Nettle"},{"Pine"},{"Pinker"},{"Plump"},{"River"},{"Rock"},{"Stock"},{"Sun"},{"Turtle"},{"Whistle"},{"Wood"}};
    static char *suffix[]={{"boro"},{"burg"},{"bury"},{"dale"},{"field"},{"ford"},{"ham"},{"thorpe"},{"ton"},{"view"},{"ville"}};

    r.Seed(seed);

    if(r.NextDouble()<0.5)
    {
        int8_t i=(r.Next()%prefixcount);
        int l=snprintf(&name[pos],lenleft,"%s ",prefix[i]);
        lenleft-=l;
        pos+=l;
    }
    if(r.NextDouble()<0.5)
    {
        int8_t i=(r.Next()%directioncount);
        int l=snprintf(&name[pos],lenleft,"%s ",direction[i]);
        lenleft-=l;
        pos+=l;
    }
    {
        int8_t i=(r.Next()%basecount);
        int l=snprintf(&name[pos],lenleft,"%s",base[i]);
        lenleft-=l;
        pos+=l;
    }
    {
        int8_t i=(r.Next()%suffixcount);
        int l=snprintf(&name[pos],lenleft,"%s",suffix[i]);
        lenleft-=l;
        pos+=l;
    }

}
