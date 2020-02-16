// DB Handler 1.0
// Made by KYS
#pragma once
#ifndef DB_H_DEFINED
#define DB_H_DEFINED
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <utility>
#include <cstring>
#include <string>
#include <chrono>

class StringToken;
class FxException;
class FileHandler;

typedef bool (*cmprFuncPtr)(std::string, void*, int);

class DBHandler{
    public:
        bool db_open(std::string fname, std::string schema_name);
        bool db_close(std::string schema_name);
        bool db_save(std::string schema_name, std::string fname = "");
        bool is_db_open(std::string schema_name);
        std::vector<std::tuple<int, std::string> > sql_execute(std::string schema_name, std::string query);
        int find_name_row(std::string schema_name, std::string row_name);
        int find_name_type(std::string schema_name, std::string row_name);
        std::string csv_interpret(std::string csv_line);
        std::string csv_interpret_org(std::string csv_line);
        std::vector<std::string> tokenize_getline_db(const std::string& data, const char delimiter = ',');

    private:
        std::map<std::string, std::vector<std::tuple<int, std::string, std::string> > > schema_type;
        //schema_name, row<index, name, type>> ...
        std::map<std::string, std::vector<std::tuple<int, std::string> > > db_list;
        //schema_name, row<row_num, file_row[1,2..]> ...
        std::map<std::string, std::tuple<std::string> > db_file_info;
        //schema_name, row<file_name>

        std::vector<std::string> open_file(std::string fname, std::string schema_name);
        bool analyze_structure(std::vector<std::string> file_vector, std::string schema_name);
        std::vector<std::tuple<int, std::string> > compare_var(std::string schema_name, std::vector<std::tuple<int, std::string> > str_list, std::string s_key, void *s_value, int type, std::string delimiter);
        bool compare_value(std::string org, void *cmp, cmprFuncPtr func, int delim) {return func(org, cmp, delim);}

        StringToken* st;
        FxException* fe;
        FileHandler* fh;
};

bool cmp_var_int(std::string original, void* cmp, int delim);
bool cmp_var_str(std::string original, void* cmp, int delim);
bool cmp_var_flo(std::string original, void* cmp, int delim);
bool cmp_var_dou(std::string original, void* cmp, int delim);
bool cmp_var_boo(std::string original, void* cmp, int delim);


// Public Sector
bool DBHandler::db_open(std::string fname, std::string schema_name)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    std::vector<std::string> file_vector;
    file_vector = open_file(fname, schema_name);
    this->db_file_info.insert(make_pair(schema_name, std::make_tuple(fname)));
    analyze_structure(file_vector, schema_name);
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
    return true;
}

bool DBHandler::db_close(std::string schema_name)
{
    this->db_save(schema_name);
    this->schema_type.erase(schema_name);
    this->db_list.erase(schema_name);
    this->db_file_info.erase(schema_name);
    return true;
}

bool DBHandler::db_save(std::string schema_name, std::string fname)
{
    if(fname.compare("") == 0)
    {
        fname = std::get<0>(this->db_file_info[schema_name]);
    }

    fh->write(fname,""); //clear file
    std::string s_name = "", s_type = "";

    for(int i=0; i<this->schema_type[schema_name].size(); i++)
    {
        s_name += std::get<1>(this->schema_type[schema_name][i]) + ",";
        s_type += std::get<2>(this->schema_type[schema_name][i]) + ",";
    }
    s_name.erase(s_name.end()-1,s_name.end());
    s_type.erase(s_type.end()-1,s_type.end());
    fh->write_relay(fname,s_name + "\n");
    fh->write_relay(fname,s_type + "\n");

    for(int i=0; i < this->db_list[schema_name].size(); i++)
    {
        fh->write_relay(fname, this->csv_interpret_org(std::get<1>(db_list[schema_name][i])) + "\n");
    }

    return true;
}

bool DBHandler::is_db_open(std::string schema_name)
{
    return (schema_type.find(schema_name) == schema_type.end()) ? false : true;
}

std::vector<std::tuple<int, std::string> > DBHandler::sql_execute(std::string schema_name, std::string query)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    std::vector<std::tuple<int, std::string> > return_vector;

    if(query.find('"') != std::string::npos) //is_" char
    {
        query = this->csv_interpret(query);
    }

    if(query.find("SELECT") != std::string::npos)
    {
        std::vector<std::string> tk;
        tk = st->tokenize_getline(query, ' ');
        bool query_vaildation = false;
        for(int i = 1; i < tk.size(); i++)
        {
            if(tk[i].find("WHERE(") != std::string::npos)
            {
                std::string qs = st->erase_str(tk[i], "WHERE(");
                qs.erase(qs.end()-1, qs.end()); 

                std::vector<std::string> where_s;
                where_s = st->tokenize_getline(qs, '|');
                for(int j = 0; j < where_s.size(); j++)
                {
                    std::vector<std::string> key_val;
                    std::string delimiter = "";
                    if(where_s[j].find("<=") != std::string::npos)
                        delimiter = "<=";
                    else if(where_s[j].find(">=") != std::string::npos)
                        delimiter = ">=";
                    else if(where_s[j].find("!=") != std::string::npos)
                        delimiter = "!=";
                    else if(where_s[j].find("<") != std::string::npos)
                        delimiter = "<";
                    else if(where_s[j].find(">") != std::string::npos)
                        delimiter = ">";
                    else if(where_s[j].find("==") != std::string::npos)
                        delimiter = "==";
                    else if(where_s[j].find("=") != std::string::npos)
                        delimiter = "=";
                    else
                        throw(Exceptions(E_ERROR,"Syntax Error. Unable to Find delimiter."));
                    
                    key_val = st->tokenize_getline_str(where_s[j], delimiter);

                    if(key_val.size() < 2){
                        throw(Exceptions(E_ERROR,"Syntax Error. Split Failed."));
                        return return_vector;
                    }
                    
                    if(j == 0)
                    {
                        if((key_val[0] == "*") && (key_val[1] == "*"))
                            return this->db_list[schema_name];
                        return_vector = this->db_list[schema_name];
                    }

                    return_vector = this->compare_var(schema_name, return_vector, key_val[0], static_cast<void*>(&key_val[1]), find_name_type(schema_name, key_val[0]), delimiter);
                }

                query_vaildation = true;
            }
        }

        if(query_vaildation == false)
            throw(Exceptions(E_ERROR, "Syntax Error. WHERE(...) should be included."));
    }
    else if(query.find("INSERT") != std::string::npos)
    {
        //insert
        //INSERT VALUES(a,b,c,...)
        std::vector<std::string> tk;
        tk = st->tokenize_getline(query, ' ');
        for(int i = 1; i < tk.size(); i++)
        {
            if(tk[i].find("VALUES(") != std::string::npos)
            {
                std::string qs = st->erase_str(tk[i], "VALUES(");
                qs.erase(qs.end()-1, qs.end());

                db_list[schema_name].push_back(std::make_tuple(std::get<0>(db_list[schema_name].back()) + 1, qs));
                return_vector.push_back(std::make_tuple(200,"Success"));
            }
        }
    }
    else if(query.find("UPDATE") != std::string::npos)
    {
        //update
        //UPDATE SET(a=b|c=d) WHERE(a=c|b=d)

        std::vector<std::tuple<int, std::string> > update_row;
        std::vector<std::tuple<int, std::string> > select_w;
        std::vector<std::string> tmp_s;
        std::string tmp_ss;

        std::vector<std::string> tk;
        tk = st->tokenize_getline(query, ' ');
        bool query_vaildation[2] = {false};

        for(int i = 1; i < tk.size(); i++)
        {
            if(tk[i].find("SET(") != std::string::npos)
            {
                std::string qs = st->erase_str(tk[i], "SET(");
                qs.erase(qs.end()-1, qs.end());

                std::vector<std::string> where_s, tmp_v;
                int rs;
                where_s = st->tokenize_getline(qs, '|');
                for(int j = 0; j < where_s.size(); j++)
                {
                    tmp_v = st->tokenize_getline(where_s[j], '=');
                    rs = find_name_row(schema_name, tmp_v[0]);
                    update_row.push_back(std::make_pair(rs, tmp_v[1]));
                }

                query_vaildation[0] = true;
            }

            if(tk[i].find("WHERE(") != std::string::npos)
            {
                std::string sql_q = "SELECT "+tk[i];
                select_w = this->sql_execute(schema_name, sql_q);

                query_vaildation[1] = true;
            }
        }

        if(query_vaildation[0] == false){throw(Exceptions(E_ERROR, "Syntax Error. SET(...) should be included."));}
        else if(query_vaildation[1] == false){throw(Exceptions(E_ERROR, "Syntax Error. WHERE(...) should be included."));}

        for(int i = 0; i < select_w.size(); i++)
        {
            int rs = std::get<0>(select_w[i]);

            if(rs != std::get<0>(db_list[schema_name][rs - 1]))
                throw(Exceptions(E_ERROR, "Finding Error"));

            tmp_s = st->tokenize_getline(this->csv_interpret(std::get<1>(db_list[schema_name][rs - 1])), ',');

            for(int j = 0; j < update_row.size(); j++){
                tmp_s[std::get<0>(update_row[j])] = std::get<1>(update_row[j]);
            }

            for(int j = 0; j < tmp_s.size(); j++){
                tmp_ss.append(tmp_s[j] + ",");
            }

            std::get<1>(db_list[schema_name][rs - 1]) = tmp_ss.substr(0, tmp_ss.length() - 1);
            tmp_ss = "";
        }
    }
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start; //Query Time

    return return_vector;
}

int DBHandler::find_name_row(std::string schema_name, std::string row_name)
{
    typedef std::vector<std::tuple<int, std::string, std::string> > dbs;
    
    for (dbs::const_iterator i = this->schema_type[schema_name].begin(); i != this->schema_type[schema_name].end(); ++i) {
        if(row_name.compare(std::get<1>(*i)) == 0)
        {
            return std::get<0>(*i);
        }
    }

    throw(Exceptions(E_ERROR, "Can not find row name ["+row_name+"]."));
    return -1;
}

int DBHandler::find_name_type(std::string schema_name, std::string row_name)
{
    typedef std::vector<std::tuple<int, std::string, std::string> > dbs;
    
    for (dbs::const_iterator i = this->schema_type[schema_name].begin(); i != this->schema_type[schema_name].end(); ++i) {
        if(row_name.compare(std::get<1>(*i)) == 0)
        {
            std::string type_name = std::get<2>(*i);
            int return_int = 0;

            if(type_name.compare("int") == 0)
                return_int = 0;
            else if(type_name.compare("string") == 0)
                return_int = 1;
            else if(type_name.compare("float") == 0)
                return_int = 2;
            else if(type_name.compare("double") == 0)
                return_int = 3;
            else if(type_name.compare("bool") == 0)
                return_int = 4;
            else
                throw(Exceptions(E_ERROR, "Unknown schema type."));

            return return_int;
        }
    }

    throw(Exceptions(E_ERROR, "Can not find type name ["+row_name+"]."));
    return -1;
}

std::vector<std::string> DBHandler::tokenize_getline_db(const std::string& data, const char delimiter) {
	std::vector<std::string> result;
	std::string token;
	std::stringstream ss(data);

	while (getline(ss, token, delimiter)) {
		result.push_back(this->csv_interpret_org(token));
	}
	return result;
}


// Private Sector
std::vector<std::string> DBHandler::open_file(std::string fname, std::string schema_name)
{
    int i = 0;
    std::vector<std::string> file_vector;
    std::string str;
    std::ifstream readFile;
    readFile.open(fname);

    if(readFile.is_open())
    {
        while(!readFile.eof())
        {
            getline(readFile, str);
            if(i < 2)
                file_vector.push_back(str);
            else{
                if(str.find('"') != std::string::npos) //is_" char
                {
                    str = this->csv_interpret(str);
                }
            
                if(trim(str) == "") // empty line
                    break;
                
                db_list[schema_name].push_back(std::make_tuple(i-1, str));
            }
            i++;
        }
        readFile.close();
    }else{
        throw(Exceptions(E_ERROR,"File Read Failed"));
    }
    return file_vector;
}

bool DBHandler::analyze_structure(std::vector<std::string> file_vector, std::string schema_name)
{
    std::vector<std::string> sch_name, sch_type;
    std::vector<std::tuple<int, std::string, std::string> > structs;

    for(int i = 0; i < 2; i++)
    {
        std::string s = file_vector[i];
        std::vector<std::string> str = st->tokenize_getline(s, ',');
        for(int j = 0; j < str.size(); j++)
        {
            if(i == 0)
                sch_name.push_back(trim(str[j]));
            else
                sch_type.push_back(trim(str[j]));
        }
    }

    for(int i = 0; i < sch_name.size(); i++)
    {
        structs.push_back(std::tuple<int, std::string, std::string> (i, sch_name[i], sch_type[i]));
    }

    this->schema_type.insert(make_pair(schema_name, structs));
    return true;
}

std::vector<std::tuple<int, std::string> > DBHandler::compare_var(std::string schema_name, std::vector<std::tuple<int, std::string> > str_list, std::string s_key, void *s_value, int type, std::string delimiter = "=")
{
    cmprFuncPtr cmpf = NULL;
    
    switch(type)
    {
        // type
        // 0 : int
        // 1 : string
        // 2 : float
        // 3 : double
        // 4 : bool
        
        case 0:
        cmpf = cmp_var_int;
        break;

        case 1:
        cmpf = cmp_var_str;
        break;

        case 2:
        cmpf = cmp_var_flo;
        break;

        case 3:
        cmpf = cmp_var_dou;
        break;

        case 4:
        cmpf = cmp_var_boo;
        break;

        default:
        cmpf = cmp_var_str;
        throw(Exceptions(E_WARNING, "Compare type invaild."));
        break;
    }

    int delim = -1;
    if(delimiter.find("<=") != std::string::npos)
        delim = 0;
    else if(delimiter.find(">=") != std::string::npos)
        delim = 1;
    else if(delimiter.find("!=") != std::string::npos)
        delim = 2;
    else if(delimiter.find("<") != std::string::npos)
        delim = 3;
    else if(delimiter.find(">") != std::string::npos)
        delim = 4;
    else if(delimiter.find("=") != std::string::npos)
        delim = 5;
    else
        throw(Exceptions(E_ERROR,"Syntax Error. Unable to Find delimiter."));
    
    int row_cnt = this->find_name_row(s_key, schema_name);
    
    std::vector<std::string> tmp_str;
    std::vector<std::tuple<int, std::string> > return_vector;

    if(row_cnt < 0)
        return return_vector;

    for(int i = 0; i < str_list.size(); i++)
    {
        tmp_str = st->tokenize_getline(std::get<1>(str_list[i]), ',');

        //std::cout << tmp_str[row_cnt] << ":" << *(static_cast<std::string*>(s_value)) <<std::endl;
        if(this->compare_value(tmp_str[row_cnt], s_value, cmpf, delim))
        {
            return_vector.push_back(std::make_tuple(std::get<0>(str_list[i]), std::get<1>(str_list[i])));
        }
    }

    return return_vector;
}

std::string DBHandler::csv_interpret(std::string csv_line)
{
    bool isquote = false;
    for(int j = 0; j < csv_line.size() - 1; j++)
    {
        try{
            if((csv_line.at(j) == '"') && (csv_line.at(j+1) == '"') && isquote) //inquote
            {
                csv_line[j] = 6;
                csv_line[j+1] = 6;
                j++;
                continue;
            }

            if((csv_line.at(j) == ' ') && isquote)
            {
                csv_line[j] = 7;
            }

            if((csv_line.at(j) == '"') && !isquote)
            {
                isquote = true;
                continue;
            }

            if((csv_line.at(j) == '"') && (csv_line.at(j+1) != '"') && isquote) //inquote
            {
                isquote = false;
                continue;
            }

            if((csv_line.at(j) == ',') && isquote)
            {
                csv_line[j] = 5;
            }
        }catch(std::exception &a){
            throw(Exceptions(E_ERROR, "Parsing Error."));
        }
    }

    return csv_line;
}

std::string DBHandler::csv_interpret_org(std::string csv_line)
{
    for(int j = 0; j < csv_line.size() - 1; j++)
    {
        try{
            if((csv_line.at(j) == 6) && (csv_line.at(j+1) == 6)) //inquote
            {
                csv_line.replace(j,2, "\"\"");
                j++;
                continue;
            }

            if((csv_line.at(j) == 7))
            {
                csv_line[j] = ' ';
            }

            if((csv_line.at(j) == 5))
            {
                csv_line[j] = ',';
            }
        }catch(std::exception &a){
            throw(Exceptions(E_ERROR, "Parsing Error."));
        }
    }

    return csv_line;
}

bool cmp_var_int(std::string original, void* cmp, int delim) {
    int org = stoi(original), cp = stoi(*(static_cast<std::string*>(cmp)));
    switch(delim){
        case 0:
        return (org <= cp) ? true : false; break;
        case 1:
        return (org >= cp) ? true : false; break;
        case 2:
        return (org != cp) ? true : false; break;
        case 3:
        return (org < cp) ? true : false; break;
        case 4:
        return (org > cp) ? true : false; break;
        case 5:
        return (org == cp) ? true : false; break;
        default:
        throw(Exceptions(E_ERROR, "operator error."));
    }
}
bool cmp_var_str(std::string original, void* cmp, int delim) {
    bool rtn = (original.compare(*(static_cast<std::string*>(cmp))) == 0) ? true : false;
    if(delim = 5) return rtn; else if(delim = 2) return !rtn; else throw(Exceptions(E_ERROR, "operator error. string can compare only '=' or '!='."));
}
bool cmp_var_flo(std::string original, void* cmp, int delim) {
    float org = stof(original), cp = stof(*(static_cast<std::string*>(cmp)));
    switch(delim){
        case 0:
        return (org <= cp) ? true : false; break;
        case 1:
        return (org >= cp) ? true : false; break;
        case 2:
        return (org != cp) ? true : false; break;
        case 3:
        return (org < cp) ? true : false; break;
        case 4:
        return (org > cp) ? true : false; break;
        case 5:
        return (org == cp) ? true : false; break;
        default:
        throw(Exceptions(E_ERROR, "operator error."));
    }
}
bool cmp_var_dou(std::string original, void* cmp, int delim) {
    double org = stod(original), cp = stod(*(static_cast<std::string*>(cmp)));
    switch(delim){
        case 0:
        return (org <= cp) ? true : false; break;
        case 1:
        return (org >= cp) ? true : false; break;
        case 2:
        return (org != cp) ? true : false; break;
        case 3:
        return (org < cp) ? true : false; break;
        case 4:
        return (org > cp) ? true : false; break;
        case 5:
        return (org == cp) ? true : false; break;
        default:
        throw(Exceptions(E_ERROR, "operator error."));
    }
}
bool cmp_var_boo(std::string original, void* cmp, int delim) {
    bool org =  ((strcasecmp("true", original.c_str()) == 0) || stoi(original) != 0) ? true : false;
    int cp = stoi(*(static_cast<std::string*>(cmp)));
    bool rtn = ((cp > 0) && org) ? true : false;
    if(delim = 5) return rtn; else if(delim = 2) return !rtn; else throw(Exceptions(E_ERROR, "operator error. boolean can compare only '=' or '!='."));
}

#endif