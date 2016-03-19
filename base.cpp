#include <bits/stdc++.h>
using namespace std;

#define forn(i, n) for (int i = 0; i < (int)(n); ++i)

const int maxn = 10505;
typedef pair<int, int> pii;
#define se second
#define fi first
#define pb push_back
#define eb emplace_back
#define sz(x) ((int)(x).size())

const int minx = -648000;
const int maxx = 647999;
const int modx = 648000 * 2; //360 deg
const int miny = -324000;
const int maxy = 324000;
const int mody = 324000 * 2; //180 deg

int normx(int a) {
    while (a < minx)
        a += modx;
    while (a > maxx)
        a -= modx;
    return a;
}

struct pt {
    int x, y;

    pt atSlow(int v, int t) {
        int x = this->x, y = this->y;
        forn (i, t) {
            if (miny <= y + v && y + v <= maxy) {
                y += v;
                x = normx(x - 15);
            }
            else if (y + v > maxy) {
                y = mody - (y + v);
                x = normx(-modx / 2 + x - 15);
                v = -v;
            }
            else if (y + v < miny) {
                y = -mody - (y + v);
                x = normx(-modx / 2 + x - 15);
                v = -v;
            }
        }
        return pt{x, y};
    }

    pt operator-(const pt &p) const {
        int dy = y - p.y;
        int dx = normx(x - p.x);
        return pt{dx, dy};
    }

    bool operator==(const pt& other) const {
        return x == other.x && y == other.y;
    }
};

pt readPoint() {
    int x, y;
    scanf("%d%d", &x, &y);
    return {y, x};
}


int T;
int curTime;

int nsat;
pt satPosition[maxn];
int satSpeed[maxn], camSpeed[maxn], camRange[maxn];
pt camPosition[maxn];
int lastMove[maxn];

struct Collection {
    int value;
    vector<pt> locs;
    vector<pii> times;

    void removeLoc(const pt& loc) {
        auto it = find(locs.begin(), locs.end(), loc);
        assert(it != locs.end());
        locs.erase(it);
    }

    bool available() const {
        for (const pii& be: times) {
            if (be.fi <= curTime && curTime <= be.se) {
                return true;
            }
        }
        return false;
    }
};

int ncol;
Collection cols[maxn];

void scan() {
    scanf("%d", &T);

    scanf("%d", &nsat);
    forn(i, nsat) {
        satPosition[i] = readPoint();
        scanf("%d%d%d", &satSpeed[i], &camSpeed[i], &camRange[i]);
    }

    scanf("%d", &ncol);
    forn(i, ncol) {
        int k, r;
        scanf("%d%d%d", &cols[i].value, &k, &r);
        forn(j, k) {
            cols[i].locs.pb(readPoint());
        }
        forn(j, r) {
            int b, e;
            scanf("%d%d", &b, &e);
            cols[i].times.eb(b, e);
        }
    }

    std::cerr << "T = " << T << endl;

    /*
    fprintf(stderr,"%d sattelites\n", nsat);
    forn(i, nsat) {
        fprintf(stderr,"speed: %d  cam: %d  range: %d  (max time to move: %d)\n",
            satSpeed[i], camSpeed[i], camRange[i], (camRange[i] + camSpeed[i] - 1) / camSpeed[i]);
    }
    fprintf(stderr,"\n");

    fprintf(stderr,"%d collections\n", ncol);
    sort(cols, cols + ncol, [](const Collection& lhs, const Collection& rhs) {
        return lhs.value < rhs.value;
    });
    forn(i, ncol) {
        fprintf(stderr,"$%d, %d locations, %d time ranges\n",
            cols[i].value, sz(cols[i].locs), sz(cols[i].times));
        int totalLen = 0;
        for (pii be: cols[i].times) {
            totalLen += be.se - be.fi + 1;
        }
        fprintf(stderr,"Time avail: %d/%d (%.2lf%%)\n", totalLen, T, totalLen * 100. / T);
    }
    */
}

vector<tuple<int, int, int, int>> result;
void writeOperation(int satId, pt pt) {
    result.eb(pt.y, pt.x, satId, curTime);
}

void dumpOutput() {
    printf("%d\n", sz(result));
    for (auto xxxx: result) {
        printf("%d %d %d %d\n", get<0>(xxxx), get<1>(xxxx), get<2>(xxxx), get<3>(xxxx));
    }
}


void advanceSatellites() {
    ++curTime;
    forn(i, nsat) {
        satPosition[i] = satPosition[i].atSlow(satSpeed[i], 1);
    }
}

inline int divideBy(int x, int y) {
    return (abs(x) + y - 1) / y;
}

bool canTakePhoto(int satId, pt pos) {
    pt dist = pos - satPosition[satId];
    if (abs(dist.x) > camRange[satId] || abs(dist.y) > camRange[satId]) {
        return false;
    }
    pt relDist = dist - camPosition[satId];
    int timeToMove = max(divideBy(relDist.x, camSpeed[satId]), divideBy(relDist.y, camSpeed[satId]));
    return lastMove[satId] + timeToMove <= curTime;
}

void solve() {
    while (curTime <= T && ncol > 0) {
        forn(satId, nsat) {
            for (int i = ncol - 1; i >= 0; --i) {
                if (!cols[i].available()) {
                    continue;
                }
                bool done = false;
                for (pt pos: cols[i].locs) {
                    if (canTakePhoto(satId, pos)) {
                        done = true;
                        writeOperation(satId, pos);
                        cols[i].removeLoc(pos);

                        lastMove[satId] = curTime;
                        camPosition[satId] = pos - satPosition[satId];

                        done = true;
                        break;
                    }
                }

                if (done) {
                    break;
                }
            }
        }

        advanceSatellites();
//         std::cerr << "curTime = " << curTime << std::endl;
    }

    dumpOutput();
}

int main() {
    scan();
    solve();
}
