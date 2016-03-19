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

struct pt {
    int x, y;

    // operations: TODO
};

pt readPoint() {
    int x, y;
    scanf("%d%d", &x, &y);

    // TODO: preprocess
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
