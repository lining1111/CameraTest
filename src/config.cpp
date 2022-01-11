//
// Created by lining on 1/5/22.
//

#include <cstdio>
#include "config.h"
#include "sqlite3.h"
#include <string>

using namespace std;

static int CallbackGetBase(void *data, int argc, char **argv, char **azColName) {
    string colName;
    if (data != nullptr) {
        auto baseInfoP = (BASE_INFO *) data;
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("width") == 0) {
                baseInfoP->width = atoi(argv[i]);
            }
            if (colName.compare("height") == 0) {
                baseInfoP->height = atoi(argv[i]);
            }
        }
    } else {
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("width") == 0) {
                base_info.width = atoi(argv[i]);
            }
            if (colName.compare("height") == 0) {
                base_info.height = atoi(argv[i]);
            }
        }
    }

    return 0;
}

static int CallbackGetDistance(void *data, int argc, char **argv, char **azColName) {
    string colName;
    if (data != nullptr) {
        auto baseInfoP = (BASE_INFO *) data;
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("x_distance") == 0) {
                baseInfoP->x_distance = strtod(argv[i], nullptr);
            }
            if (colName.compare("y_distance") == 0) {
                baseInfoP->y_distance = strtod(argv[i], nullptr);
            }
            if (colName.compare("altitude") == 0) {
                baseInfoP->H = strtod(argv[i], nullptr);
            }
            if (colName.compare("coefficient") == 0) {
                baseInfoP->threshold_value = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix00") == 0) {
                baseInfoP->perspective_trans_fa[0][0] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix01") == 0) {
                baseInfoP->perspective_trans_fa[0][1] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix02") == 0) {
                baseInfoP->perspective_trans_fa[0][2] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix10") == 0) {
                baseInfoP->perspective_trans_fa[1][0] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix11") == 0) {
                baseInfoP->perspective_trans_fa[1][1] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix12") == 0) {
                baseInfoP->perspective_trans_fa[1][2] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix20") == 0) {
                baseInfoP->perspective_trans_fa[2][0] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix21") == 0) {
                baseInfoP->perspective_trans_fa[2][1] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix22") == 0) {
                baseInfoP->perspective_trans_fa[2][2] = strtod(argv[i], nullptr);
            }
            if (colName.compare("radar_x") == 0) {
                baseInfoP->radar_x = strtod(argv[i], nullptr);
            }
            if (colName.compare("radar_y") == 0) {
                baseInfoP->radar_y = strtod(argv[i], nullptr);
            }
            if (colName.compare("radar_theta") == 0) {
                baseInfoP->radar_theta = strtod(argv[i], nullptr);
            }
            if (colName.compare("MPPW") == 0) {
                baseInfoP->MPPW = strtod(argv[i], nullptr);
            }
            if (colName.compare("MPPH") == 0) {
                baseInfoP->MPPH = strtod(argv[i], nullptr);
            }

        }
    } else {
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("x_distance") == 0) {
                base_info.x_distance = strtod(argv[i], nullptr);
            }
            if (colName.compare("y_distance") == 0) {
                base_info.y_distance = strtod(argv[i], nullptr);
            }
            if (colName.compare("altitude") == 0) {
                base_info.H = strtod(argv[i], nullptr);
            }
            if (colName.compare("coefficient") == 0) {
                base_info.threshold_value = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix00") == 0) {
                base_info.perspective_trans_fa[0][0] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix01") == 0) {
                base_info.perspective_trans_fa[0][1] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix02") == 0) {
                base_info.perspective_trans_fa[0][2] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix10") == 0) {
                base_info.perspective_trans_fa[1][0] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix11") == 0) {
                base_info.perspective_trans_fa[1][1] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix12") == 0) {
                base_info.perspective_trans_fa[1][2] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix20") == 0) {
                base_info.perspective_trans_fa[2][0] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix21") == 0) {
                base_info.perspective_trans_fa[2][1] = strtod(argv[i], nullptr);
            }
            if (colName.compare("matrix22") == 0) {
                base_info.perspective_trans_fa[2][2] = strtod(argv[i], nullptr);
            }
            if (colName.compare("radar_x") == 0) {
                base_info.radar_x = strtod(argv[i], nullptr);
            }
            if (colName.compare("radar_y") == 0) {
                base_info.radar_y = strtod(argv[i], nullptr);
            }
            if (colName.compare("radar_theta") == 0) {
                base_info.radar_theta = strtod(argv[i], nullptr);
            }
            if (colName.compare("MPPW") == 0) {
                base_info.MPPW = strtod(argv[i], nullptr);
            }
            if (colName.compare("MPPH") == 0) {
                base_info.MPPH = strtod(argv[i], nullptr);
            }
        }
    }

    return 0;
}

static int CallbackGetVibrate_setting(void *data, int argc, char **argv, char **azColName) {
    string colName;
    if (data != nullptr) {
        auto baseInfoP = (BASE_INFO *) data;
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("x_vibrate_max") == 0) {
                baseInfoP->x_vibrate_max = strtod(argv[i], nullptr);
            }
            if (colName.compare("y_vibrate_max") == 0) {
                baseInfoP->y_vibrate_max = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxX") == 0) {
                baseInfoP->matchBox.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxY") == 0) {
                baseInfoP->matchBox.y = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxWidth") == 0) {
                baseInfoP->matchBox.width = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxHeight") == 0) {
                baseInfoP->matchBox.height = strtod(argv[i], nullptr);
            }
        }
    } else {
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("x_vibrate_max") == 0) {
                base_info.x_vibrate_max = strtod(argv[i], nullptr);
            }
            if (colName.compare("y_vibrate_max") == 0) {
                base_info.y_vibrate_max = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxX") == 0) {
                base_info.matchBox.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxY") == 0) {
                base_info.matchBox.y = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxWidth") == 0) {
                base_info.matchBox.width = strtod(argv[i], nullptr);
            }
            if (colName.compare("matchBoxHeight") == 0) {
                base_info.matchBox.height = strtod(argv[i], nullptr);
            }
        }
    }

    return 0;
}

static int CallbackGetCrossing_setting(void *data, int argc, char **argv, char **azColName) {
    string colName;
    if (data != nullptr) {
        auto baseInfoP = (BASE_INFO *) data;
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("orientations") == 0) {
                baseInfoP->orientations = atoi(argv[i]);
            }
            if (colName.compare("deltax_south") == 0) {
                baseInfoP->deltax_south = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_south") == 0) {
                baseInfoP->deltay_south = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltax_north") == 0) {
                baseInfoP->deltax_north = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_north") == 0) {
                baseInfoP->deltay_north = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltax_west") == 0) {
                baseInfoP->deltax_west = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_west") == 0) {
                baseInfoP->deltay_west = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltax_east") == 0) {
                baseInfoP->deltax_east = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_east") == 0) {
                baseInfoP->deltay_east = strtod(argv[i], nullptr);
            }
            if (colName.compare("flag_south") == 0) {
                baseInfoP->flag_south = atoi(argv[i]);
            }
            if (colName.compare("flag_north") == 0) {
                baseInfoP->flag_north = atoi(argv[i]);
            }
            if (colName.compare("flag_west") == 0) {
                baseInfoP->flag_west = atoi(argv[i]);
            }
            if (colName.compare("flag_east") == 0) {
                baseInfoP->flag_east = atoi(argv[i]);
            }
            if (colName.compare("widthX") == 0) {
                baseInfoP->widthX = strtod(argv[i], nullptr);
            }
            if (colName.compare("widthY") == 0) {
                baseInfoP->widthY = strtod(argv[i], nullptr);
            }
        }
    } else {
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            if (colName.compare("orientations") == 0) {
                base_info.orientations = atoi(argv[i]);
            }
            if (colName.compare("deltax_south") == 0) {
                base_info.deltax_south = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_south") == 0) {
                base_info.deltay_south = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltax_north") == 0) {
                base_info.deltax_north = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_north") == 0) {
                base_info.deltay_north = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltax_west") == 0) {
                base_info.deltax_west = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_west") == 0) {
                base_info.deltay_west = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltax_east") == 0) {
                base_info.deltax_east = strtod(argv[i], nullptr);
            }
            if (colName.compare("deltay_east") == 0) {
                base_info.deltay_east = strtod(argv[i], nullptr);
            }
            if (colName.compare("flag_south") == 0) {
                base_info.flag_south = atoi(argv[i]);
            }
            if (colName.compare("flag_north") == 0) {
                base_info.flag_north = atoi(argv[i]);
            }
            if (colName.compare("flag_west") == 0) {
                base_info.flag_west = atoi(argv[i]);
            }
            if (colName.compare("flag_east") == 0) {
                base_info.flag_east = atoi(argv[i]);
            }
            if (colName.compare("widthX") == 0) {
                base_info.widthX = strtod(argv[i], nullptr);
            }
            if (colName.compare("widthY") == 0) {
                base_info.widthY = strtod(argv[i], nullptr);
            }
        }
    }

    return 0;
}

static int CallbackGetReal_loc(void *data, int argc, char **argv, char **azColName) {
    string colName;
    if (data != nullptr) {
        auto baseInfoP = (BASE_INFO *) data;
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            //left
            if (colName.compare("real_left_point_x") == 0) {
                baseInfoP->real_left_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_left_point_y") == 0) {
                baseInfoP->real_left_point.y = strtod(argv[i], nullptr);
            }
            //right
            if (colName.compare("real_right_point_x") == 0) {
                baseInfoP->real_right_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_right_point_y") == 0) {
                baseInfoP->real_right_point.y = strtod(argv[i], nullptr);
            }
            //top
            if (colName.compare("real_top_point_x") == 0) {
                baseInfoP->real_top_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_top_point_y") == 0) {
                baseInfoP->real_top_point.y = strtod(argv[i], nullptr);
            }
            //bottom
            if (colName.compare("real_bottom_point_x") == 0) {
                baseInfoP->real_bottom_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_bottom_point_y") == 0) {
                baseInfoP->real_bottom_point.y = strtod(argv[i], nullptr);
            }
        }
    } else {
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            //left
            if (colName.compare("real_left_point_x") == 0) {
                base_info.real_left_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_left_point_y") == 0) {
                base_info.real_left_point.y = strtod(argv[i], nullptr);
            }
            //right
            if (colName.compare("real_right_point_x") == 0) {
                base_info.real_right_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_right_point_y") == 0) {
                base_info.real_right_point.y = strtod(argv[i], nullptr);
            }
            //top
            if (colName.compare("real_top_point_x") == 0) {
                base_info.real_top_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_top_point_y") == 0) {
                base_info.real_top_point.y = strtod(argv[i], nullptr);
            }
            //bottom
            if (colName.compare("real_bottom_point_x") == 0) {
                base_info.real_bottom_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("real_bottom_point_y") == 0) {
                base_info.real_bottom_point.y = strtod(argv[i], nullptr);
            }
        }
    }

    return 0;
}

static int CallbackGetPixel_loc(void *data, int argc, char **argv, char **azColName) {
    string colName;
    if (data != nullptr) {
        auto baseInfoP = (BASE_INFO *) data;
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            //left
            if (colName.compare("pixel_left_point_x") == 0) {
                baseInfoP->pixel_left_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_left_point_y") == 0) {
                baseInfoP->pixel_left_point.y = strtod(argv[i], nullptr);
            }
            //right
            if (colName.compare("pixel_right_point_x") == 0) {
                baseInfoP->pixel_right_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_right_point_y") == 0) {
                baseInfoP->pixel_right_point.y = strtod(argv[i], nullptr);
            }
            //top
            if (colName.compare("pixel_top_point_x") == 0) {
                baseInfoP->pixel_top_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_top_point_y") == 0) {
                baseInfoP->pixel_top_point.y = strtod(argv[i], nullptr);
            }
            //bottom
            if (colName.compare("pixel_bottom_point_x") == 0) {
                baseInfoP->pixel_bottom_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_bottom_point_y") == 0) {
                baseInfoP->pixel_bottom_point.y = strtod(argv[i], nullptr);
            }
        }
    } else {
        for (int i = 0; i < argc; i++) {
            colName = string(azColName[i]);
            //left
            if (colName.compare("pixel_left_point_x") == 0) {
                base_info.pixel_left_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_left_point_y") == 0) {
                base_info.pixel_left_point.y = strtod(argv[i], nullptr);
            }
            //right
            if (colName.compare("pixel_right_point_x") == 0) {
                base_info.pixel_right_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_right_point_y") == 0) {
                base_info.pixel_right_point.y = strtod(argv[i], nullptr);
            }
            //top
            if (colName.compare("pixel_top_point_x") == 0) {
                base_info.pixel_top_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_top_point_y") == 0) {
                base_info.pixel_top_point.y = strtod(argv[i], nullptr);
            }
            //bottom
            if (colName.compare("pixel_bottom_point_x") == 0) {
                base_info.pixel_bottom_point.x = strtod(argv[i], nullptr);
            }
            if (colName.compare("pixel_bottom_point_y") == 0) {
                base_info.pixel_bottom_point.y = strtod(argv[i], nullptr);
            }
        }
    }

    return 0;
}


BASE_INFO base_info;

int read_real_loc_info(char *filename) {
    sqlite3 *db;
    char *errmsg = nullptr;

    //open
    int rc = sqlite3_open(filename, &db);
    if (rc != SQLITE_OK) {
        printf("sqlite open fail\n");
        sqlite3_close(db);
    }

    //base
    char *sqlGetBase = "select * from base";
    rc = sqlite3_exec(db, sqlGetBase, CallbackGetBase, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite err:%s\n", errmsg);
        sqlite3_free(errmsg);
    }
    //distance
    char *sqlGetDistance = "select * from distance";
    rc = sqlite3_exec(db, sqlGetDistance, CallbackGetDistance, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite err:%s\n", errmsg);
        sqlite3_free(errmsg);
    }

    //vibrate_setting
    char *sqlGetVibrate_setting = "select * from vibrate_setting";
    rc = sqlite3_exec(db, sqlGetVibrate_setting, CallbackGetVibrate_setting, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite err:%s\n", errmsg);
        sqlite3_free(errmsg);
    }

    //crossing_setting
    char *sqlGetCrossing_setting = "select * from crossing_setting";
    rc = sqlite3_exec(db, sqlGetCrossing_setting, CallbackGetCrossing_setting, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite err:%s\n", errmsg);
        sqlite3_free(errmsg);
    }

    //real_loc
    char *sqlGetReal_loc = "select * from real_loc";
    rc = sqlite3_exec(db, sqlGetReal_loc, CallbackGetReal_loc, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite err:%s\n", errmsg);
        sqlite3_free(errmsg);
    }


    //pixel_loc
    char *sqlGetPixel_loc = "select * from pixel_loc";
    rc = sqlite3_exec(db, sqlGetPixel_loc, CallbackGetPixel_loc, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite err:%s\n", errmsg);
        sqlite3_free(errmsg);
    }


    sqlite3_close(db);
    return 0;
}