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
#define all(x) (x).begin(), (x).end()

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

    pt atSlow(int &v, int t) {
        int x = this->x, y = this->y;
        forn (i, t) {
            if (miny <= y + v && y + v <= maxy) {
                y += v;
                x = normx(x - 15);
            }
            else {
                if (y + v > maxy) {
                    y = mody - (y + v);
                } else {
                    y = -mody - (y + v);
                }
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

    bool operator<(const pt& other) const {
        return tie(x, y) < tie(other.x, other.y);
    }
};

ostream& operator<<(ostream& out, const pt& p) {
    return out << p.x << " " << p.y;
}

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
            auto p = readPoint();
            if (!count(all(cols[i].locs), p)) {
                cols[i].locs.pb(p);
            }
        }
        forn(j, r) {
            int b, e;
            scanf("%d%d", &b, &e);
            cols[i].times.eb(b, e);
        }
    }

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

int score = 0;

vector<tuple<int, int, int, int>> result;
void writeOperation(int satId, pt pt) {
    result.eb(pt.y, pt.x, curTime, satId);
}

void dumpOutput() {
    printf("%d\n", sz(result));
    for (auto xxxx: result) {
        printf("%d %d %d %d\n", get<0>(xxxx), get<1>(xxxx), get<2>(xxxx), get<3>(xxxx));
    }

    cerr << "SCORE: " << score << endl;
}

set<int> availX;
set<pii> availY[modx + 100]; // {y, compId}
set<pii>& yset(int x) {
    return availY[x + maxx + 50];
}

void buildSpatialIndex() {
    forn(i, ncol) {
        for (const auto& pt: cols[i].locs) {
            availX.insert(pt.x);
            yset(pt.x).emplace(pt.y, i);
        }
    }

    cerr << "different x: " << availX.size() << endl;
    int total = 0;
    for (int x: availX) {
        total += yset(x).size();
    }
    cerr << "total points: " << total << endl;
}

vector<pair<pt, int>> closePoints(pt origin, int dist) {
    if (availX.empty()) return {};

    auto smartNext = [](set<int>::iterator it) {
        if (++it == availX.end()) {
            return availX.begin();
        }
        return it;
    };
    auto smartPrev = [](set<int>::iterator it) {
        if (it == availX.begin()) {
            return --availX.end();
        }
        return --it;
    };

    vector<pair<pt, int>> res;
    auto xit = availX.lower_bound(origin.x);
    if (xit == availX.begin()) {
        xit = --availX.end();
    }
    bool passedAll = false;

    for (auto it = xit;; it = smartNext(it)) {
        if (abs(*it - origin.x) > dist) {
            break;
        }

        for (const auto& y_comp: yset(*it)) {
            if (abs(y_comp.fi - origin.y) <= dist) {
                res.emplace_back(pt{*it, y_comp.fi}, y_comp.se);
            }
        }

//         if (it == xit) {
//             passedAll = true;
//             break;
//         }
    }

    if (!passedAll) {
        for (auto it = smartPrev(xit);; it = smartPrev(it)) {
            if (abs(*it - origin.x) > dist) {
                break;
            }

            for (const auto& y_comp: yset(*it)) {
                if (abs(y_comp.fi - origin.y) <= dist) {
                    res.emplace_back(pt{*it, y_comp.fi}, y_comp.se);
                }
            }
        }
    }

    return res;
}

void advanceSatellites() {
    ++curTime;
    forn(i, nsat) {
        satPosition[i] = satPosition[i].atSlow(satSpeed[i], 1);
    }
}

void removeCollection(int colId) {
    --ncol;
    for (int i = colId; i < ncol; ++i) {
        cols[i] = cols[i + 1];
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
            auto closePt = closePoints(satPosition[satId], camRange[satId]);
            if (closePt.empty()) {
                continue;
            }
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
                    if (cols[i].locs.empty()) {
                        cerr << "done collection " << i << " @" << curTime << ", $" << cols[i].value << "\n";
                        score += cols[i].value;
                        removeCollection(i);
                    }
                    break;
                }
            }
        }

        advanceSatellites();
        if (curTime % 1000 == 0) {
            cerr << "timestamp: " << curTime << ", elapsed: " << clock()/1000 << " ms" << endl;
        }
    }

    dumpOutput();
}

int main() {
    scan();
    buildSpatialIndex();
    solve();

    cerr << "Time: " << clock()/1000 << " ms" << endl;
}
