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

pt initPosition[maxn];

struct Collection {
    int value;
    vector<pt> locs;
    vector<pt> init;
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

    int totalTime() const {
        int t = 0;
        for (auto kv: times) {
            t += kv.se - kv.fi + 1;
        }
        return t;
    }

    void reset() {
        locs = init;
    }
};

int ncol;
Collection cols[maxn];

void scan() {
    scanf("%d", &T);

    scanf("%d", &nsat);
    forn(i, nsat) {
        initPosition[i] = readPoint();
        satPosition[i] = initPosition[i];
        scanf("%d%d%d", &satSpeed[i], &camSpeed[i], &camRange[i]);
    }

    scanf("%d", &ncol);
    forn(i, ncol) {
        int k, r;
        scanf("%d%d%d", &cols[i].value, &k, &r);
        int x = 1000000000, X = -1000000000;
        int y = 1000000000, Y = -1000000000;
        forn(j, k) {
            auto p = readPoint();
            if (!count(all(cols[i].locs), p)) {
                cols[i].locs.pb(p);
                x = min(x, p.x);
                X = max(X, p.x);
                y = min(y, p.y);
                Y = max(Y, p.y);
            }
        }
//         cerr << max(X-x, Y-y) << "\n";
        forn(j, r) {
            int b, e;
            scanf("%d%d", &b, &e);
            cols[i].times.eb(b, e);
        }
        cols[i].init = cols[i].locs;
    }

    fprintf(stderr,"%d sattelites\n", nsat);
    forn(i, nsat) {
        fprintf(stderr,"speed: %d  cam: %d  range: %d  (max time to move: %d)\n",
            satSpeed[i], camSpeed[i], camRange[i], (camRange[i] + camSpeed[i] - 1) / camSpeed[i]);
    }
    fprintf(stderr,"\n");
    /*

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
int totalPoints = 0;

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
    totalPoints = total;
}

void rebuildAll() {
    forn(i, nsat) {
        satPosition[i] = initPosition[i];
        camPosition[i] = {0,0};
    }

    curTime = 0;

    for (int x: availX) {
        yset(x).clear();
    }
    availX.clear();

    buildSpatialIndex();

    result.clear();
    score = 0;

    forn(i, ncol) {
        cols[i].reset();
    }
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

double value(const pair<pt, int> &a) {
    return a.second;
//     assert(!cols[a.second].locs.empty());
//     return cols[a.second].value;// / (double) sz(cols[a.second].locs);
}

bool cmp(const pair<pt, int> &a, const pair<pt, int> &b) {
    return value(a) < value(b);
}

bool badComp[maxn];

void solve() {
    memset(badComp, 0, sizeof badComp);

    while (true) {
        while (curTime <= T && ncol > 0) {
            forn(satId, nsat) {
                auto closePt = closePoints(satPosition[satId], camRange[satId]);
                if (closePt.empty()) {
                    continue;
                }

                vector<pair<pt, int>> candidates;
                for (auto pp: closePt) {
                    int i = pp.second;
                    pt pos = pp.first;
                    if (find(cols[i].locs.begin(), cols[i].locs.end(), pos) == cols[i].locs.end())
                        continue;
                    if (!cols[i].available())
                        continue;
                    if (!canTakePhoto(satId, pos))
                        continue;
                    candidates.push_back(pp);
                }

                if (candidates.empty())
                    continue;

                pair<pt, int> where = *min_element(all(candidates), cmp);
                int i = where.second;
                pt pos = where.first;

                writeOperation(satId, pos);
                cols[i].removeLoc(pos);
                lastMove[satId] = curTime;
                camPosition[satId] = pos - satPosition[satId];
                if (cols[i].locs.empty()) {
                    cerr << "done collection " << i << " @" << curTime << ", $" << cols[i].value << "\n";
                    score += cols[i].value;
                }
            }

            advanceSatellites();
            if (curTime % 1000 == 0) {
    //             cerr << "timestamp: " << curTime << ", elapsed: " << clock()/1000 << " ms" << endl;
            }
        }

        int doneCols = 0;
        int leftPoints = 0;
        forn(i, ncol) {
            doneCols += (cols[i].locs.empty());
            leftPoints += cols[i].locs.size();

            badComp[i] = !cols[i].locs.empty() && cols[i].locs.size() < cols[i].init.size();
        }

        cerr << "Components done: " << doneCols << " of " << ncol << endl;
        cerr << "Points killed: " << totalPoints - leftPoints << " of " << totalPoints << endl;
        cerr << "Score: " << score << endl;

        int old_nc = ncol;
        for (int i = 0; i < ncol; ++i) {
            if (badComp[i]) {
                swap(cols[i], cols[ncol - 1]);
                --ncol;
                --i;
            }
        }
        if (old_nc == ncol) {
            break;
        }
        rebuildAll();
        cerr << "rebuilt" << endl;
    }

    dumpOutput();
}

int main() {
    scan();
    buildSpatialIndex();
    solve();

    cerr << "Time: " << clock()/1000 << " ms" << endl;
}
