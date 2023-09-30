//
//  main.cpp
//  Bluejay-Delivery
//
//  Created by Kush Singh on 29/09/23.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

const chrono::hours SEVEN_DAYS(7 * 24);
const chrono::hours ONE_HOUR(1);
const chrono::hours TEN_HOURS(10);
const chrono::hours FOURTEEN_HOURS(14);

chrono::system_clock::time_point parse_datetime(const string &date_str)
{
    tm tm = {};
    istringstream ss(date_str);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return chrono::system_clock::from_time_t(mktime(&tm));
}

int main()
{
    ifstream file("Assignment_Timecard.xlsx - Sheet1.csv");
    if (!file.is_open())
    {
        cerr << "Failed to open the input file." << endl;
        return 1;
    }

    string line;
    getline(file, line);

    size_t name_idx = string::npos;
    size_t position_idx = string::npos;
    size_t timestamp_idx = string::npos;

    istringstream header(line);
    string header_token;
    size_t token_idx = 0;
    while (getline(header, header_token, ','))
    {
        if (header_token == "Name")
        {
            name_idx = token_idx;
        }
        else if (header_token == "Position")
        {
            position_idx = token_idx;
        }
        else if (header_token == "Timestamp")
        {
            timestamp_idx = token_idx;
        }
        token_idx++;
    }

    cerr << "Header row: " << line << endl;

    string previous_employee;
    int consecutive_days = 0;

    while (getline(file, line))
    {
        istringstream row(line);
        string token;
        vector<string> tokens;

        while (getline(row, token, ','))
        {
            tokens.push_back(token);
        }

        if (tokens.size() > max({name_idx, position_idx, timestamp_idx}))
        {
            string name = tokens[name_idx];
            string position = tokens[position_idx];
            chrono::system_clock::time_point timestamp = parse_datetime(tokens[timestamp_idx]);

            if (previous_employee.empty() || name != previous_employee)
            {
                consecutive_days = 0;
            }

            if (timestamp - parse_datetime(tokens[timestamp_idx - 1]) <= SEVEN_DAYS)
            {
                consecutive_days++;
            }
            else
            {
                consecutive_days = 0;
            }

            auto time_between_shifts = chrono::duration_cast<chrono::hours>(timestamp - parse_datetime(tokens[timestamp_idx - 1]));
            if (time_between_shifts > ONE_HOUR && time_between_shifts < TEN_HOURS)
            {
                cout << name << ": " << position << " - Less than 10 hours between shifts" << endl;
            }

            if (time_between_shifts > FOURTEEN_HOURS)
            {
                cout << name << ": " << position << " - Shift exceeds 14 hours" << endl;
            }

            if (consecutive_days == 7)
            {
                cout << name << ": " << position << " - Worked 7 consecutive days" << endl;
            }

            previous_employee = name;
        }
    }

    file.close();
    return 0;
}
