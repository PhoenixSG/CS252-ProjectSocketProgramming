#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
#define repi(n) for (ll i = 0; i < n; i++)
#define repi1(n) for (ll i = 1; i <= n; i++)
#define repk(n) for (ll k = 0; k < n; k++)
#define repk1(n) for (ll k = 1; k <= n; k++)
#define repj(n) for (ll j = 0; j < n; j++)
#define repj1(n) for (ll j = 1; j <= n; j++)
#define repii(start, finish, step) for (ll i = start; i <= finish; i += step)
#define repiiq(start, finish, step) for (ll i = start; i >= finish; i += step)
#define repjjq(start, finish, step) for (ll j = start; j >= finish; j += step)
#define repkkq(start, finish, step) for (ll k = start; k >= finish; k += step)
#define repjj(start, finish, step) for (ll j = start; j <= finish; j += step)
#define repkk(start, finish, step) for (ll k = start; k <= finish; k += step)
#define in(a)    \
    cin >> temp; \
    a.push_back(temp)
#define vint vector<int>
#define vll vector<ll>
#define N 1000000007
#define L 200005
#define y() cout << "YES\n"
#define n() cout << "NO\n"
#define p(x) cout << x << "\n"
// #define p(x, y) cout << x << y << "\n";

void solve()
{
    ll n, m;

    cin>>n;

    ll a[n];
    set<ll> s;

    repi(n){
        cin>>a[i];
        s.insert(a[i]);
    }
    if(s.size()==1){
        p(1);
        repi(n){
            cout<<"1 ";
        }
        p("");
        return;
    }
    else{
        if(n%2==0){
            p(2);
            repi(n/2){
                cout<<"1 2 ";
            }
            p("");
            return;
        }
        else{

            map<ll,set<ll>> g;

            repi(n){
                g[a[i%n]].insert(a[(i+1)%n]);
                g[a[(i+1)%n]].insert(a[i%n]);
            }
            for(auto x:g){
                if(x.second.find(x.first)!=x.second.end()){
                    p(2);
                    bool flag = true;
                    repi(n){
                        if(i<n-1 && flag && a[i%n]==a[(i+1)%n] ){
                            cout<<i%2+1<<" ";
                            cout<<i%2+1<<" ";
                            n--;
                            flag = false;
                            continue;
                        }
                        cout<<i%2+1<<" ";
                    }
                    p("");
                    return;
                }
            }
            p(3);
            repi(n/2){
                cout<<"1 2 ";
            }
            p(3);
            return;
        }
    }



}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    ll t = 1;

    cin >> t;

    repj(t)
    {
        solve();
    }
    cout.flush();
    return 0;
}
