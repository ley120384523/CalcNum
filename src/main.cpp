
#pragma warning(disable: 4819)

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread_pool.hpp>

using namespace std;
using namespace boost;

typedef unsigned int uint;

unsigned int boundary_num(const vector<pair<uint, uint>>& nums, int calc_num, bool upper_boundary = true);

void combine(uint m, uint n, list<vector<uint>>& species);

int main(int argc, char* argv[])
{
    int calc_num = 0;
    cout << "输入想求的数字：";
#ifndef _DEBUG
    cin >> calc_num;
#endif
    cout << "开始计算！";

    // 解析文本
    vector<string> fields;
    {
        ifstream st("新建文本文档.txt");
        string txt((istreambuf_iterator<char>(st)), istreambuf_iterator<char>());
        trim(txt);
        split(fields, txt, is_any_of("\n"));
    }

    // row -> num
    uint field_index = 1;
    vector<pair<uint, uint>> nums;
    for (const string& _x : fields)
    {
        nums.push_back(std::make_pair(field_index, lexical_cast<int>(_x.c_str())));
        ++field_index;
    }
    std::sort(nums.begin(), nums.end(), [](const std::pair<uint, uint>& _left, const std::pair<uint, uint>& _right) { return _left.second < _right.second; });

    uint upper_num = boundary_num(nums, calc_num);          // 最多
    uint lower_num = boundary_num(nums, calc_num, false);   // 最少

    // 生成组合
    list<vector<uint>> species;
    {
        thread_pool pool;
        for (uint i = lower_num; i <= upper_num; ++i)
            pool.submit(bind(&combine, nums.size(), i, boost::ref(species)));
    }

    // 所有结果
    // sum -> rows
    list<pair<int, vector<uint>>> sums;
    for (const vector<uint>& _o : species)
        sums.push_back(make_pair(accumulate(_o.begin(), _o.end(), 0, [&](uint _s, uint _x) { _s += nums[_x].second; return _s; }), _o));
    // 组合排序
    sums.sort([&](const pair<int, vector<uint>>& left, pair<int, vector<uint>>& right) { return abs(left.first - calc_num) < abs(right.first - calc_num); });

    // 写入结果
    ofstream of("result.csv");
    for (const pair<int, vector<uint>>& _n : sums)
    {
        // vector<int> => vector<string>
        vector<std::string> pix;
        for (uint _x : _n.second)   pix.push_back(lexical_cast<string>(nums[_x].first));

        // 写入文本
        of << _n.first << ", (" << algorithm::join(pix, ", ") << ")" << endl;
    }

    return 0;
}

unsigned int boundary_num(const vector<pair<uint, uint>>& nums, int calc_num, bool upper_boundary /*= true*/)
{
    int num_sum = 0;
    for (uint i = 0; i < nums.size(); ++i)
    {
        num_sum += nums[upper_boundary ? i : nums.size() - i - 1].second;
        if (num_sum > calc_num) return upper_boundary ? i + 1 : (i == 0) ? 1 : i;
    }

    return 1;
}

boost::shared_mutex combine_mutex;
void combine_recursion(uint m, uint residue, uint* specie, uint n, list<vector<uint>>& species)
{
    for (int i = m - 1; i >= 0; --i)
    {
        specie[residue - 1] = i;
        if (residue > 1)
            combine_recursion(i, residue - 1, specie, n, species);
        else
        {
            vector<uint> specie_gather(n);
            for (uint j = 0; j < n; ++j)
                specie_gather[j] = specie[j];

            boost::unique_lock<boost::shared_mutex> mutex_lock(combine_mutex);
            species.push_back(specie_gather);
        }
    }
}

void combine(uint m, uint n, std::list<std::vector<uint>>& species)
{
    uint* specie = new uint[n];
    combine_recursion(m, n, specie, n, species);
    delete[] specie;
}

