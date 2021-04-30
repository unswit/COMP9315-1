#include <bits/stdc++.h>

using namespace std;
// op is the operation + for request, - for release
// page is the name of the page
char op[2], page[5], strategy[5];
map<string, int> pageTobuf;
map<int, string> bufTopage;
map<int, int> bufTotime;
map<int, int> timeTobuf;
map<int, int> pincount;
set<int> unused;

void inc(int &t) {
    if (strcmp(strategy, "MRU") == 0) {
        t++;
    } else {
        t--;
    }
}

void show() {
    printf("%s %s\n", op, page);
    for (auto iter : bufTopage) {
        printf("[%d (%d)]: %s\n", iter.first, pincount[iter.first], iter.second.c_str());
    }
    printf("%s: ", strategy);
    auto iter = timeTobuf.rbegin();
    while (iter != timeTobuf.rend()) {
        printf("%d->", iter->second);
        iter++;
    }
    printf("X\n\n");
}

int main() {
    int i, nbuf, reqrel = 0;
    scanf("%d%s%d", &nbuf, strategy, &reqrel);
    for (i = 0 ; i < nbuf; ++i) {
        unused.insert(i);
    }    
    
    int hit = 0, tolreq = 0, timestamp = 0;
    while (scanf("%s%s", op, page) != EOF) {
        string curr = string(page);
        int bufid = -1;
        if (op[0] == '+') {
            tolreq++;
            if (!unused.empty()) {
                int v = *unused.begin();
                pageTobuf[curr] = v;
                bufTopage[v] = curr;
                if (reqrel) {
                    timeTobuf[timestamp] = v;
                    bufTotime[v] = timestamp;
                }
                pincount[v] = 1;
                unused.erase(v);
                inc(timestamp);
                show();
                continue;
            }

            if (pageTobuf.find(curr) != pageTobuf.end()) {
                printf("we got a hit when request %s\n", curr.c_str());
                ++hit;
                bufid = pageTobuf[curr];
                pincount[bufid] = 1;
                timeTobuf.erase(bufTotime[bufid]);
                if (reqrel) {
                    bufTotime[bufid] = timestamp;
                    timeTobuf[timestamp] = bufid;
                }  
                inc(timestamp);
                show();
                continue;
            }
            
            auto iter = timeTobuf.rbegin();
            while (iter != timeTobuf.rend()) {
                if (!pincount[iter->second]) {
                    bufid = iter->second;
                    break;
                }
                iter++;
            }
            
            if (bufid == -1) {
                printf("insufficient buffer!\n");
                return 0;
            }
            timeTobuf.erase(bufTotime[bufid]);
            pageTobuf.erase(bufTopage[bufid]);
            pageTobuf[curr] = bufid;
            bufTopage[bufid] = curr;
            if (reqrel) {
                bufTotime[bufid] = timestamp;
                timeTobuf[timestamp] = bufid;
            }
            pincount[bufid]++;
        } else {
            if (pageTobuf.find(curr) == pageTobuf.end()) {
                printf("invalid release, page %s not requested!\n", curr.c_str());
                return 0;
            }

            bufid = pageTobuf[curr];
            pincount[bufid]--;
            timeTobuf.erase(bufTotime[bufid]);
            bufTotime[bufid] = timestamp;
            timeTobuf[timestamp] = bufid;    
        }
        show();
        inc(timestamp);
    }

    printf("total page request is %d, total hit is %d\n", tolreq, hit);
    return 0;
}