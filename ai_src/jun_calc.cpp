#include "jun_calc.hpp"
#include <string>
#include <vector>
#include <algorithm> // For std::max
#include <fstream>   // For std::ifstream

// Platform-specific includes for executable path retrieval
#ifdef _WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h> // For PATH_MAX
#elif __linux__
#include <unistd.h> // For readlink
#include <limits.h> // For PATH_MAX
#else
#ifndef PATH_MAX
#define PATH_MAX 1024 // Default PATH_MAX for other/unknown systems
#endif
#endif

namespace AkochanPathUtils {

// Function to get the full path of the current executable
std::string get_executable_full_path() {
    char buffer[PATH_MAX];
#ifdef _WIN32
    if (GetModuleFileNameA(NULL, buffer, PATH_MAX) == 0) {
        return ""; // Error retrieving path
    }
#elif __APPLE__
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        return ""; // Error retrieving path
    }
#elif __linux__
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (count == -1 || count == PATH_MAX) {
        return ""; // Error retrieving path
    }
    buffer[count] = '\0'; // Null-terminate the string
#else
    return ""; 
#endif
    return std::string(buffer);
}

// Function to get the directory containing the executable
std::string get_executable_directory() {
    std::string exec_full_path = get_executable_full_path();
    if (exec_full_path.empty()) {
        return "."; // Fallback
    }

    size_t last_separator_pos;
#ifdef _WIN32
    size_t last_bslash = exec_full_path.rfind('\\');
    size_t last_fslash = exec_full_path.rfind('/');
    if (last_bslash == std::string::npos) last_separator_pos = last_fslash;
    else if (last_fslash == std::string::npos) last_separator_pos = last_bslash;
    else last_separator_pos = std::max(last_bslash, last_fslash);
#else
    last_separator_pos = exec_full_path.rfind('/');
#endif

    if (last_separator_pos != std::string::npos) {
        return exec_full_path.substr(0, last_separator_pos);
    }
    return ".";
}

// Simple file existence check
bool check_file_exists(const std::string& file_path) {
    std::ifstream stream(file_path.c_str());
    return stream.good();
}

// Determine the prefix for accessing data files ("params" directory)
std::string get_params_directory_prefix() {
    static std::string cached_prefix = "";
    static bool is_initialized = false;
    if (is_initialized) {
        return cached_prefix;
    }

    std::string exec_dir = get_executable_directory();
    std::string path_separator_char = "/";
#ifdef _WIN32
    path_separator_char = "\\";
#endif

    const std::string params_folder_name = "params";
    const std::string check_file_in_params = params_folder_name + path_separator_char + "rank_prob" + path_separator_char + "ako" + path_separator_char + "para4_9000.txt";

    std::string current_try_prefix = exec_dir + path_separator_char;
    if (check_file_exists(current_try_prefix + check_file_in_params)) {
        cached_prefix = current_try_prefix;
        is_initialized = true;
        return cached_prefix;
    }

    if (exec_dir != "." && !exec_dir.empty() && exec_dir.find(path_separator_char) != std::string::npos) {
        size_t last_sep_for_parent_dir = exec_dir.rfind(path_separator_char);
        if (last_sep_for_parent_dir != std::string::npos) {
            std::string parent_directory = exec_dir.substr(0, last_sep_for_parent_dir);
            current_try_prefix = parent_directory + path_separator_char;
            if (check_file_exists(current_try_prefix + check_file_in_params)) {
                cached_prefix = current_try_prefix;
                is_initialized = true;
                return cached_prefix;
            }
        }
    }
    
    current_try_prefix = ""; 
    if (check_file_exists(current_try_prefix + check_file_in_params)) {
        cached_prefix = current_try_prefix;
        is_initialized = true;
        return cached_prefix;
    }
    
    if (exec_dir != "." && !exec_dir.empty()) {
         cached_prefix = exec_dir + path_separator_char;
    } else {
        cached_prefix = "";
    }
    
    is_initialized = true;
    return cached_prefix;
}

} // namespace AkochanPathUtils

std::array<std::array<float, 4>, 4> calc_jun_prob_end(const std::array<int, 4>& ten, const int oya_first) {
    std::array<std::array<float, 4>, 4> jun_prob;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			jun_prob[i][j] = 0.0;
		}
	}
	std::vector<Player_Result> results;
	for (int j = 0; j < 4; j++) {
		results.push_back(Player_Result(j, ten[j], (4 + j - oya_first)%4 ));
	}
	std::sort(results.begin(), results.end());
	for (int j = 0; j < 4; j++) {
		jun_prob[results[j].pid][j] = 1.0;
	}
    return jun_prob;
}

std::array<int, 16> score_list_to_4onehot(const std::array<int, 4>& score) {
    std::vector<Player_Result> results;
	for (int i = 0; i < 4; i++) {
		results.push_back(Player_Result(i, score[i], i));
	}
	std::sort(results.begin(), results.end());
    
    std::array<int, 16> onehot;
    std::fill(onehot.begin(), onehot.end(), 0);
    for (int j = 0; j < 4; j++) {
        onehot[4*j + results[j].pid] = 1;
    }
    return onehot;
}

std::array<float, 24> infer_game_result_prob_ako(const std::array<int, 4>& score, const int kyoku_idx) {
	float w[96];
	std::string base_path_prefix = AkochanPathUtils::get_params_directory_prefix();
	const std::string relative_file_path = kyoku_idx < 8 ?
		"params/rank_prob/ako/para" + std::to_string(kyoku_idx) + "_9000.txt" :
		"params/rank_prob/ako/para" + std::to_string(kyoku_idx) + "_nn.txt";
	
	const std::string absolute_file_name = base_path_prefix + relative_file_path;

	read_parameters(w, 96, absolute_file_name); // Use the new absolute path
	float x[4];
	x[0] = 1.0;
	for(int i=1;i<=3;i++){
		x[i] = ((float)(score[0] - score[i]))/10000.0;
	}
	float pk[24];
	MC_logistic(w, x, pk, 4, 24);

	std::array<float, 24> output;
	for (int i = 0; i < 24; i++) {
		output[i] = pk[i];
	}
	return output;
}

// オーラスで親がアガリまたはテンパイ流局した場合にゲームが終わるか判定
bool chan_end_oya(const std::array<int, 4>& ten, const int oyaid){
	if(ten[oyaid]>=30000){
		for(int pid=0;pid<4;pid++){
			if(pid==oyaid){
				continue;
			}else if(ten[pid]>=ten[oyaid]){
				return false;
			}
		}
		return true;
	}else{
		return false;
	}
}

// 西４で親がアガリまたはテンパイ流局した場合にゲームが終わるか判定
bool chan_end_oya_w4(const std::array<int, 4>& ten, const int oyaid) {
	for (int pid = 0; pid < 4; pid ++) {
		if (pid == oyaid) {
			continue;
		} else if (ten[pid] >= ten[oyaid]) {
			return false;
		}
	}
	return true;
}

std::array<std::array<float, 4>, 4> calc_jun_prob(const int kyoku, const std::array<int,4>& ten, const int oya, const bool is_renchan, const json11::Json& tactics_json) {
	const int oya_first = (12 + oya - kyoku) % 4;
	if (is_renchan) {
		if ((kyoku >= 7 && chan_end_oya(ten, oya)) ||
			(kyoku == 11 && chan_end_oya_w4(ten, oya))
		) {
			return calc_jun_prob_end(ten, oya_first);
		}
	}

	if (tactics_json["jun_est"] == "instant") {
		return calc_jun_prob_end(ten, oya_first);
	}

	std::array<std::array<float, 4>, 4> jun_prob, jun_prob_tmp;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4;j++) {
			jun_prob[i][j] = 0.0;
			jun_prob_tmp[i][j] = 0.0;
		}
	}

	if((kyoku<8 && ten[0]>=0 && ten[1]>=0 && ten[2]>=0 && ten[3]>=0)
		|| (kyoku<12 && ten[0]>=0 && ten[1]>=0 && ten[2]>=0 && ten[3]>=0 && ten[0]<30000 && ten[1]<30000 && ten[2]<30000 && ten[3]<30000)
	){
		std::array<int, 4> x;
		for (int i = 0; i < 4; i++) {
			x[i] = ten[(oya_first + i) % 4];
		}

		const std::array<float, 24> pk = [&] {
			// to do. kyoku_mod_next = 4 の時、データが不足しているせいでモデルが良くない。kyoku_mod_next = 4 の時は5にしてしまうか、東南戦をまじめにやるか。
			if (tactics_json["jun_est"] == "ako") { return infer_game_result_prob_ako(x, kyoku); }
			else {
				assert_with_out(false, "cal_kyoku_end_pt_exp_error");
				return infer_game_result_prob_ako(x, kyoku);
			}
		}();

		int jun[4];
		for (int i = 0; i < 24; i++) {
			for (int j = 0; j < 4; j++) {
				jun[j] = j;
			}
			for(int m=0;m<3;m++){
				int k = (i%factorial(4-m))/factorial(3-m);
				int tmp = jun[k+m];
				for(int n=0;n<k;n++){
					jun[k-n+m] = jun[k-n+m-1];
				}
				jun[m] = tmp;
			}
			//printf("junc:%d %d %d %d %d %lf\n", i, jun[0], jun[1], jun[2], jun[3], pk[i]);

			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					if (jun[k] == j) {
						jun_prob_tmp[j][k] += pk[i];
					}
				}
			}
		}
		for (int pid = 0; pid < 4; pid++) {
			for (int i = 0; i < 4; i++) {
				jun_prob[pid][i] = jun_prob_tmp[mod_pid(kyoku, oya, pid)][i];
				//printf("%d %d %d %d %d %lf\n", kyoku, oya, pid, i, mod_pid(kyoku, oya, pid), jun_prob[pid][i]);
			}
		}
		return jun_prob;
	} else {
		return calc_jun_prob_end(ten, oya_first);
	}
}
