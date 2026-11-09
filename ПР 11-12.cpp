// Задача 1
/*
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

int main() {
    int n, m;
    cin >> n >> m;

    // Карта: a[i][j] — баллы в клетке (i, j)
    vector<vector<long long>> a(n, vector<long long>(m));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            cin >> a[i][j];

    // dp[i][j] — максимальная сумма баллов на пути из (0,0) в (i,j)
    // Двигаться можно только вниз и вправо, поэтому в клетку (i,j)
    // приходим либо сверху (i-1,j), либо слева (i,j-1).
    vector<vector<long long>> dp(n, vector<long long>(m, 0));

    // База: стартовая клетка
    dp[0][0] = a[0][0];

    // Первый столбец: только сверху
    for (int i = 1; i < n; ++i)
        dp[i][0] = dp[i-1][0] + a[i][0];

    // Первая строка: только слева
    for (int j = 1; j < m; ++j)
        dp[0][j] = dp[0][j-1] + a[0][j];

    // Основная рекуррентность: берём лучший из двух путей + текущая клетка
    for (int i = 1; i < n; ++i)
        for (int j = 1; j < m; ++j)
            dp[i][j] = max(dp[i-1][j], dp[i][j-1]) + a[i][j];

    // ---- Восстановление маршрута с конца в начало ----
    vector<pair<int,int>> path;
    int i = n - 1, j = m - 1;
    while (i != 0 || j != 0) {
        path.push_back({i, j});
        // Идём туда, откуда пришли с большим значением dp.
        // Если значения равны — предпочитаем шаг вверх (i-1) для определённости.
        if (i > 0 && (j == 0 || dp[i-1][j] >= dp[i][j-1]))
            --i;
        else
            --j;
    }
    path.push_back({0, 0});                 // обязательная стартовая точка
    reverse(path.begin(), path.end());      // разворачиваем в прямой порядок

    // Вывод результата
    cout << "Max sum: " << dp[n-1][m-1] << "\nPath:\n";
    for (auto [x, y] : path)
        cout << x << " " << y << "\n";

    return 0;
}
    */
// Задача 2
/*
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <deque>
using namespace std;

// Проверка: можно ли разбить массив c на <= D непрерывных отрезков так,
// чтобы в каждом разброс (max - min) был <= X.
bool can(const vector<long long>& c, int D, long long X) {
    int n = c.size();
    const int INF = 1e9;

    // dp[i] — минимальное число отрезков для первых i элементов
    vector<int> dp(n + 1, INF);
    dp[0] = 0;

    // Для текущего правого конца i поддерживаем окно [left..i],
    // в котором spread(c[left..i]) <= X.
    // max и min на окне — через два монотонных deque.
    deque<int> maxq, minq;   // индексы элементов
    int left = 0;

    // dp[j] для всех j в окне [left..i] — храним в multiset,
    // чтобы быстро брать минимум.
    multiset<int> bestInWindow;

    for (int i = 0; i < n; ++i) {
        // 1) Расширяем окно вправо: добавляем c[i]
        while (!maxq.empty() && c[maxq.back()] <= c[i]) maxq.pop_back();
        maxq.push_back(i);
        while (!minq.empty() && c[minq.back()] >= c[i]) minq.pop_back();
        minq.push_back(i);

        // 2) В окно добавляется новый возможный старт j = i
        if (dp[i] < INF) bestInWindow.insert(dp[i]);

        // 3) Сужаем окно слева, пока разброс превышает X
        while (!maxq.empty() && !minq.empty() &&
               c[maxq.front()] - c[minq.front()] > X) {
            if (maxq.front() == left) maxq.pop_front();
            if (minq.front() == left) minq.pop_front();

            auto it = bestInWindow.find(dp[left]);
            if (it != bestInWindow.end()) bestInWindow.erase(it);
            ++left;
        }

        // 4) Переход: dp[i+1] = min(dp[j]) + 1 по всем j из окна
        if (!bestInWindow.empty())
            dp[i+1] = min(dp[i+1], *bestInWindow.begin() + 1);
    }

    return dp[n] <= D;
}

int main() {
    int N, D;
    cin >> N >> D;

    vector<long long> c(N);
    for (auto& x : c) cin >> x;

    // Бинарный поиск минимального X, при котором разбиение возможно
    long long lo = 0;
    long long hi = *max_element(c.begin(), c.end())
                 - *min_element(c.begin(), c.end());

    while (lo < hi) {
        long long mid = lo + (hi - lo) / 2;
        if (can(c, D, mid)) hi = mid;
        else                lo = mid + 1;
    }

    cout << lo << "\n";
    return 0;
}
    */
// Задача 3
/*
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

int main() {
    int N, K, L;
    if (!(cin >> N >> K >> L)) return 0;

    vector<int> cost(N);
    for (int i = 0; i < N; ++i) {
        cin >> cost[i];
    }

    // Считаем, сколько раз встречается каждая стоимость.
    // map с greater<int> сразу сортирует ключи по убыванию.
    map<int, int, greater<int>> cnt;
    for (int x : cost) {
        cnt[x]++;
    }

    // Формируем пул допустимых к выбору растений:
    // каждое значение повторяем не более L раз
    vector<int> pool;
    for (auto const& p : cnt) {
        int val = p.first;
        int count = p.second;
        int take = min(count, L);
        for (int k = 0; k < take; ++k) {
            pool.push_back(val);
        }
    }

    // Если даже из всех допустимых не набрать K растений — букет невозможен
    if ((int)pool.size() < K) {
        cout << "Impossible\n";
        return 0;
    }

    // Пул уже упорядочен по убыванию (за счёт map с greater<int>),
    // поэтому жадно берём K самых дорогих.
    long long sum = 0;
    for (int i = 0; i < K; ++i) {
        sum += pool[i];
    }

    cout << sum << "\n";
    return 0;
}
*/
// Задача 1 Практика 12
/*
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

int n, m;
vector<string> lab;
map<pair<int,int>, int> pidx;
int P = 0;
pair<int,int> startPos, exitPos;
int dr[4] = {1,-1,0,0};
int dc[4] = {0,0,1,-1};

// Массив для отслеживания посещенных клеток (чтобы не ходить по кругу)
vector<vector<bool>> visited;

// Функция возвращает макс. кол-во pdf от (i,j) до выхода
int dfs(int i, int j, int current_pdfs) {
    // Если дошли до выхода, возвращаем текущее количество собранных pdf
    if (i == exitPos.first && j == exitPos.second)
        return current_pdfs;

    visited[i][j] = true;
    int best = -1e9;

    for (int d = 0; d < 4; ++d) {
        int ni = i + dr[d], nj = j + dc[d];
        if (ni < 0 || nj < 0 || ni >= n || nj >= m) continue;
        if (lab[ni][nj] == '1') continue;   // стена
        if (visited[ni][nj]) continue;      // уже были здесь

        // Если в новой клетке pdf, увеличиваем счетчик
        int next_pdfs = current_pdfs;
        if (pidx.count({ni, nj})) {
            next_pdfs++;
        }

        best = max(best, dfs(ni, nj, next_pdfs));
    }

    visited[i][j] = false; // backtracking: разрешаем другим путям пройти здесь
    return best;
}

int main() {
    cin >> n >> m;
    lab.resize(n);
    visited.assign(n, vector<bool>(m, false));
    
    for (int i = 0; i < n; ++i) {
        cin >> lab[i];
        for (int j = 0; j < m; ++j) {
            if (lab[i][j] == 'S')      startPos = {i, j};
            else if (lab[i][j] == 'E') exitPos  = {i, j};
            else if (lab[i][j] == 'P') pidx[{i, j}] = P++;
        }
    }

    // Запускаем DFS. На старте 0 собранных pdf. 
    // (Если на старте 'P', то нужно передать 1, но у нас старт 'S')
    int res = dfs(startPos.first, startPos.second, 0);
    
    if (res < 0) cout << "No path\n";
    else         cout << res << "\n";

    return 0;
}
    */