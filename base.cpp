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
};

pt readPoint() {
    int x, y;
    scanf("%d%d", &x, &y);
    return {x, y};
}


int T;

int nsat;
pt satPosition[maxn];
int satSpeed[maxn], camSpeed[maxn], camRange[maxn];

struct Collection {
    int value;
    vector<pt> locs;
    vector<pii> times;
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

    printf("%d sattelites\n", nsat);
    forn(i, nsat) {
        printf("speed: %d  cam: %d  range: %d  (max time to move: %d)\n",
            satSpeed[i], camSpeed[i], camRange[i], (camRange[i] + camSpeed[i] - 1) / camSpeed[i]);
    }
    printf("\n");

    printf("%d collections\n", ncol);
    sort(cols, cols + ncol, [](const Collection& lhs, const Collection& rhs) {
        return lhs.value > rhs.value;
    });
    forn(i, ncol) {
        printf("$%d, %d locations, %d time ranges\n",
            cols[i].value, sz(cols[i].locs), sz(cols[i].times));
        int totalLen = 0;
        for (pii be: cols[i].times) {
            totalLen += be.se - be.fi + 1;
        }
        printf("Time avail: %d/%d (%.2lf%%)\n", totalLen, T, totalLen * 100. / T);
    }
}

int main() {
    scan();
}
