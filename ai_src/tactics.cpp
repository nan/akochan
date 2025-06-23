#include "tactics.hpp"

Tactics::Tactics(){
    set_default();
}

Tactics::Tactics(const json11::Json& tactics_json) {
    set_from_json(tactics_json);
}

void Tactics::set_common() {
    jun_pt[0] = 90;
    jun_pt[1] = 30;
    jun_pt[2] = -30;
    jun_pt[3] = -90;
    do_houjuu_discount = false;
    do_speed_modify = false;
    use_larger_at_cal_tenpai_after = false;
    betaori_compare_at_2fuuro = false;
    consider_kan = true;
    do_kyushukyuhai = true;
    use_nn_keiten_estimator = false;
    use_nn_kyoku_result_target_estimator = false;
    use_nn_kyoku_result_other_agari_estimator = false;
    use_ori_choice_exception = false;
    use_new_agari_ml = false;
    use_agari_coeff_old = false;
    use_agari_coeff_tp = false;
    use_agari_coeff_fp = false;
    modify_always = false;
    use_rule_base_at_mentu5_titoi3_shanten = true;
    use_cn_max_addition_if_honitsu = false;
    use_reach_daten_data_flag = false;
    do_tsumo_prob_dora_shift = false;
    use_no_fuuro_data_flag = false;
    use_new_other_end_prob = true;
    use_fuuro_choice_simple = false;
    use_tenpai_prob_other_zero_fuuro_exception = false;
    use_ori_choice_exception_near_lose = false;
    use_reach_tenpai_prob_other_if_other_reach = true;
    use_han_shift_at_fuuro_estimation = false;
    use_han_shift_at_fuuro_estimation2 = false;
    use_new_tenpai_est_tmp = false;
    use_ratio_tas_to_coeff = true;
    use_ori_exp_at_dp_fuuro = true;
    do_ankan_inclusive = true;
    do_kakan_inclusive = true;
    do_kan_ordinary = false;
    jun_calc_bug = false;
    use_yama_ratio_kawa_num = 100;
    use_dp_last_tsumo_num = 0;
    jun_est_type = JET_DEFAULT;
    inclusive_sn_always = 1;
    inclusive_sn_other_reach = 2;
    inclusive_sn_fp = 1;
    inclusive_sn_fp_other_reach = 2;
    reach_regression_mode_default = 1;
    reach_regression_mode_other_reach = 1;
    reach_regression_coeff_other_reach = 1.0;
    norisk_ratio_if_other_reach = 0.0;
    dora_fuuro_coeff = 1.0;
    other_end_prob_max = 1.0;
    reach_tenpai_prob_other_coeff = 1.0;
    other_end_prob_coeff_if_other_reach = 1.0;
    last_tedashi_neighbor_coeff = 1.0;
    gukei_est_coeff = 0.2;
    last_effective_ratio = 1.0;
    tsumo_num_DP_max_not_menzen = 20;

    tsumo_enumerate_additional_maximum = 0;
    tsumo_enumerate_additional_minimum = 0;
    tsumo_enumerate_additional_priority = 0.0;

    for (int han = 0; han < 14; han++) {
        for (int fu = 0; fu < 12; fu++) {
            hanfu_weight_tsumo[han][fu] = 0.0;
            hanfu_weight_ron[han][fu] = 0.0;
        }
        han_shift_prob_kan[han] = 0.0;
    }
    hanfu_weight_tsumo[3][3] = 0.4;
    hanfu_weight_tsumo[4][3] = 0.5;
    hanfu_weight_tsumo[6][3] = 0.1;

    hanfu_weight_ron[2][3] = 0.1;
    hanfu_weight_ron[3][3] = 0.5;
    hanfu_weight_ron[4][3] = 0.4;

    han_shift_prob_kan[0] = 0.1;
    han_shift_prob_kan[1] = 0.8;
    han_shift_prob_kan[2] = 0.1;

    betaori_est = "ako";
}

void Tactics::set_default(){
    set_common();
    tegawari_num[0] = 2;
    tegawari_num[1] = 2;
    tegawari_num[2] = 1;
    tegawari_num[3] = 1;
    tegawari_num[4] = 0;
    tegawari_num[5] = 0;
    tegawari_num[6] = 0;
    
    fuuro_num_max = 3;
    cn_max_addition = 0;
    enumerate_restriction = -1;
    enumerate_restriction_fp = -1;
    tsumo_enumerate_always = -1;
    tsumo_enumerate_fuuro_restriction = -1;
    tsumo_enumerate_restriction = 20000;
}

void Tactics::set_light(){
    set_common();
    tegawari_num[0] = 2;
    tegawari_num[1] = 1;
    tegawari_num[2] = 1;
    tegawari_num[3] = 0;
    tegawari_num[4] = 0;
    tegawari_num[5] = 0;
    tegawari_num[6] = 0;
    
    fuuro_num_max = 3;
    cn_max_addition = 0;
    enumerate_restriction = 50000;
    enumerate_restriction_fp = 50000;
    tsumo_enumerate_always = 5000;
    tsumo_enumerate_fuuro_restriction = 40000;
    tsumo_enumerate_restriction = 20000;
}

void Tactics::set_zero_first(){
    set_common();
    tegawari_num[0] = 2;
    tegawari_num[1] = 1;
    tegawari_num[2] = 1;
    tegawari_num[3] = 1;
    tegawari_num[4] = 0;
    tegawari_num[5] = 0;
    tegawari_num[6] = 0;
    
    fuuro_num_max = 3;
    cn_max_addition = 0;
    enumerate_restriction = 30000;
    enumerate_restriction_fp = 30000;
    tsumo_enumerate_always = 2000;
    tsumo_enumerate_fuuro_restriction = 20000;
    tsumo_enumerate_restriction = 10000;
}

// Strategy focused on aggressive play in the early stages of the game.
// Aims for quick hand completion, possibly by being more willing to open the hand (fuuro)
// and being less conservative about discards.
void Tactics::set_aggressive_early_game(){
    set_common(); // Start with common defaults

    // Prioritize speed and opening hand
    do_speed_modify = true;
    fuuro_num_max = 4; // More willing to open hand
    dora_fuuro_coeff = 1.2; // Slightly more aggressive on dora calls

    // Focus on simpler, faster hands, less deep tegawari search for higher shanten
    tegawari_num[0] = 2; // 0 shanten
    tegawari_num[1] = 1; // 1 shanten
    tegawari_num[2] = 1; // 2 shanten
    tegawari_num[3] = 0; // 3 shanten
    tegawari_num[4] = 0; // 4 shanten
    tegawari_num[5] = 0; // 5 shanten
    tegawari_num[6] = 0; // 6 shanten

    cn_max_addition = 0; // Default
    enumerate_restriction = 30000; // Default-ish
    enumerate_restriction_fp = 30000; // Default-ish
    tsumo_enumerate_always = 2000; // Default-ish
    tsumo_enumerate_fuuro_restriction = 20000; // Default-ish
    tsumo_enumerate_restriction = 10000; // Default-ish

    // Potentially be more aggressive in pushing, even if others reach
    norisk_ratio_if_other_reach = 0.1; // Slightly higher than default 0.0
    inclusive_sn_other_reach = 3; // More willing to use aggressive calculation if other reach
}

// Strategy focused on defensive play, particularly in the late game or when opponents pose a threat (e.g., have declared reach).
// Prioritizes minimizing risk of dealing into opponents' hands (betaori) and being more cautious with discards.
void Tactics::set_defensive_late_game(){
    set_common(); // Start with common defaults

    // Prioritize safety, especially against reached opponents or late game
    betaori_compare_at_2fuuro = true; // Start considering betaori sooner if opponent has 2+ fuuro
    norisk_ratio_if_other_reach = 0.0; // Default, be cautious

    // Rely less on aggressive calculations when others are reaching or it's a fuuro phase for them
    inclusive_sn_other_reach = 1;
    inclusive_sn_fp_other_reach = 1;

    // Be more sensitive to last discards from others
    last_tedashi_neighbor_coeff = 1.2;

    // Adjust jun_pt to heavily avoid 4th place if that's a goal, or be more conservative
    jun_pt[0] = 80;  // Standard
    jun_pt[1] = 20;  // Standard
    jun_pt[2] = -40; // More penalty for 3rd
    jun_pt[3] = -120;// Heavily penalize 4th

    // Standard tegawari and enumeration, focus is on risk assessment
    tegawari_num[0] = 2;
    tegawari_num[1] = 2;
    tegawari_num[2] = 1;
    tegawari_num[3] = 1;
    tegawari_num[4] = 0;
    tegawari_num[5] = 0;
    tegawari_num[6] = 0;

    fuuro_num_max = 3; // Standard
    cn_max_addition = 0; // Standard
    enumerate_restriction = -1; // Standard
    enumerate_restriction_fp = -1; // Standard
    tsumo_enumerate_always = -1; // Standard
    tsumo_enumerate_fuuro_restriction = -1; // Standard
    tsumo_enumerate_restriction = 20000; // Standard
}

void Tactics::set_from_json(const json11::Json& input_json) {
         if (input_json["base"] == "minimum") { set_zero_first(); }
	else if (input_json["base"] == "light") { set_light(); }
	else if (input_json["base"] == "default") { set_default(); }
    else if (input_json["base"] == "aggressive_early") { set_aggressive_early_game(); }
    else if (input_json["base"] == "defensive_late") { set_defensive_late_game(); }
	else { assert_with_out(false, "tactics input_json base error: Unknown base strategy provided!"); }

    if (!input_json["use_ori_exp_at_dp_fuuro"].is_null()) { use_ori_exp_at_dp_fuuro = input_json["use_ori_exp_at_dp_fuuro"].bool_value(); }
    if (!input_json["do_ankan_inclusive"].is_null()) { do_ankan_inclusive = input_json["do_ankan_inclusive"].bool_value(); }
    if (!input_json["do_kakan_inclusive"].is_null()) { do_kakan_inclusive = input_json["do_kakan_inclusive"].bool_value(); }
    if (!input_json["do_kan_ordinary"].is_null()) { do_kan_ordinary = input_json["do_kan_ordinary"].bool_value(); }

    if (!input_json["jun_pt"].is_null()) {
		json11::Json::array jun_pt_json = input_json["jun_pt"].array_items();
		assert_with_out(jun_pt_json.size() == 4, "jun_pt must have 4 elements.");
		for (int pid = 0; pid < 4; pid++) {
			jun_pt[pid] = jun_pt_json[pid].int_value();
		}
	}
    
    const float eps = 1.0e-6;
    if (!input_json["han_shift_prob_kan"].is_null()) {
        const json11::Json::array han_shift_prob = input_json["han_shift_prob_kan"].array_items();
	    assert_with_out(han_shift_prob.size() == 14, "tactics input_json han_shift_prob_kan error");
        float sum = 0.0;
        for (int han = 0; han < 14; han++) {
            han_shift_prob_kan[han] = han_shift_prob[han].number_value();
            sum += han_shift_prob_kan[han];
        }
        assert_with_out(fabs(sum - 1.0) < eps, "tactics input_json han_shift_prob_kan error sum != 1.0");
    }

    if (!input_json["hanfu_weight_tsumo"].is_null()) {
        const json11::Json::array weight_array = input_json["hanfu_weight_tsumo"].array_items();
        for (int han = 0; han < 14; han++) {
            for (int fu = 0; fu < 12; fu++) {
                hanfu_weight_tsumo[han][fu] = 0.0;
            }
        }
        for (const json11::Json weight : weight_array) {
            assert_with_out(!weight["han"].is_null(), "tactics input_json hanfu_weight_tsumo error han is null:" + weight.dump());
            assert_with_out(!weight["fu"].is_null(), "tactics input_json hanfu_weight_tsumo error fu is null:" + weight.dump());
            assert_with_out(!weight["value"].is_null(), "tactics input_json hanfu_weight_tsumo error fu is null:" + weight.dump());
            const int han = weight["han"].int_value();
            const int fu = weight["fu"].int_value();
            const float value = weight["value"].number_value();
            assert_with_out(0 < han && han < 14, "tactics input_json hanfu_weight_tsumo error invalid han:" + weight.dump());
            assert_with_out(0 < fu && fu < 14, "tactics input_json hanfu_weight_tsumo error invalid fu:" + weight.dump());
            assert_with_out(0.0 <= value && value <= 1.0 + eps, "tactics input_json hanfu_weight_tsumo error invalid value:" + weight.dump());
            hanfu_weight_tsumo[han][fu] = value;
        }
        float sum = 0.0;
        for (int han = 0; han < 14; han++) {
            for (int fu = 0; fu < 12; fu++) {
                sum += hanfu_weight_tsumo[han][fu];
            }
        }
        assert_with_out(fabs(sum - 1.0) < eps, "tactics input_json hanfu_weight_tsumo error sum != 1.0");
    }

    if (!input_json["hanfu_weight_ron"].is_null()) {
        const json11::Json::array weight_array = input_json["hanfu_weight_ron"].array_items();
        for (int han = 0; han < 14; han++) {
            for (int fu = 0; fu < 12; fu++) {
                hanfu_weight_ron[han][fu] = 0.0;
            }
        }
        for (const json11::Json weight : weight_array) {
            assert_with_out(!weight["han"].is_null(), "tactics input_json hanfu_weight_ron error han is null:" + weight.dump());
            assert_with_out(!weight["fu"].is_null(), "tactics input_json hanfu_weight_ron error fu is null:" + weight.dump());
            assert_with_out(!weight["value"].is_null(), "tactics input_json hanfu_weight_ron error fu is null:" + weight.dump());
            const int han = weight["han"].int_value();
            const int fu = weight["fu"].int_value();
            const float value = weight["value"].number_value();
            assert_with_out(0 < han && han < 14, "tactics input_json hanfu_weight_ron error invalid han:" + weight.dump());
            assert_with_out(0 < fu && fu < 14, "tactics input_json hanfu_weight_ron error invalid fu:" + weight.dump());
            assert_with_out(0.0 <= value && value <= 1.0 + eps, "tactics input_json hanfu_weight_ron error invalid value:" + weight.dump());
            hanfu_weight_ron[han][fu] = value;
        }
        float sum = 0.0;
        for (int han = 0; han < 14; han++) {
            for (int fu = 0; fu < 12; fu++) {
                sum += hanfu_weight_ron[han][fu];
            }
        }
        assert_with_out(fabs(sum - 1.0) < eps, "tactics input_json hanfu_weight_ron error sum != 1.0");
    }

    if (!input_json["betaori_est"].is_null()) {
        betaori_est = input_json["betaori_est"].string_value();
    }

}

int cal_titoi_change_num_max(const int titoi_shanten_num, const int mentu_shanten_num) {
	if (titoi_shanten_num <= mentu_shanten_num) {
		if (titoi_shanten_num <= 2) {
			return titoi_shanten_num + 1;
		} else {
			return titoi_shanten_num;
		}
	} else if (titoi_shanten_num + 1 <= mentu_shanten_num) {
		return titoi_shanten_num;
	} else {
		return 0;
	}
}

bool cal_dp_flag(const int shanten_num, const int fuuro_agari_shanten_num, const bool is_other_reach_declared, const bool is_fuuro_phase, const Tactics& tactics) {
    if (shanten_num <= tactics.inclusive_sn_always) {
        return true;
    }
    if (is_other_reach_declared) {
        if (shanten_num <= tactics.inclusive_sn_other_reach) {
            return true;
        }
    }
    if (is_fuuro_phase && shanten_num <= tactics.inclusive_sn_fp) {
        return true;
    }
    if (is_other_reach_declared && is_fuuro_phase) {
        if (shanten_num <= tactics.inclusive_sn_fp_other_reach) {
            return true;
        }
    }

    if (fuuro_agari_shanten_num <= tactics.inclusive_sn_always) {
        return true;
    }
    if (is_other_reach_declared) {
        if (fuuro_agari_shanten_num <= tactics.inclusive_sn_other_reach) {
            return true;
        }
    }
    
    if (fuuro_agari_shanten_num <= tactics.inclusive_sn_fp) {
        return true;
    }

    if (is_other_reach_declared && is_fuuro_phase) {
        if (fuuro_agari_shanten_num <= tactics.inclusive_sn_fp_other_reach) {
            return true;
        }
    }
    return false;
}
		
		

		