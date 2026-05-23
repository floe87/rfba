#include "SGP4Propagator.h"
#include <iostream>


/* ------------------------ SGP4 Propagator ----------------------------- */
SGP4Propagator::SGP4Propagator() {
}

void SGP4Propagator::calculatePositionAndVelocity(int32_t secsAfterEpoch, ECICoordinate &satPos,
                                                  ECICoordinate &satVel) {
    /**
     * Local Variables needed during calculation.
     * Static to avoid stack overflow on embedded processors
     */
    static double timeSinceEpoch, QOMS2T, S, temp1, temp2, temp3, temp4, n_0, T, large_half_axis, perigee, a_1, delta_1,
            a_0, delta_0, n_0dd, a_0dd, s_star, THETA, XI, BETA_0, ETA, C_1, C_2, C_3, C_4, C_5, D_2, D_3, D_4, M_DF,
            w_DF, OMEGA_DF, delta_w, delta_M, M_p, w, e, OMEGA, a, IL, BETA, n, a_xN, IL_L, a_yNL, IL_T, a_yN, U,
            DELTA_KEPLER, KEPLER, ecosE, esinE, e_L, p_L, r, rDOT, rfDOT, cosu, sinu, u, sin2u, cos2u, DELTA_r, DELTA_u,
            DELTA_OMEGA, DELTA_i, DELTA_rDOT, DELTA_rfDOT, r_k, u_k, OMEGA_k, i_k, r_kDOT, rf_kDOT, M_x, M_y, M_z, N_x,
            N_y, N_z, U_x, U_y, U_z, V_x, V_y, V_z, x, y, z, xdot, ydot, zdot;

    static const double MIN_PER_DAY = 1440.0;
    static const double SEC_PER_DAY = 86400.0;
    static const double TWOPI = 6.2831853;
    static const double TOTHRD = 0.66666667;
    static const double TOTHRD3 = 1.5;
    static const double TOTHRD2 = 0.33333333333333333333;
    static const double AE = 1.0;

    /* Get the WGS-84 constants */
    static double const gm = 398600.5; //in [km^3/s^2]
    static double const xkmper = 6378.137; //in [km]
    static double const xke = 60.0 / sqrt(xkmper * xkmper * xkmper / gm); //in [1/min]
    static double const ck2 = 0.5 * 0.00108262998905; //0.5 * J2
    static double const a_30 = 0.00000253215306; //-J3
    static double const ck4 = (-3.0 / 8.0) * (-0.00000161098761); //-3/8 * J4

    /* Avoid time independent processing by checking the omm*/
    if (!neConstsInitialized) {
        /**
         * Will be changed if perigee < 156km == (q_0 - s)^4 (er)^4, see [2.0].
         */
        QOMS2T = pow((120.0 - 78.0) / xkmper, 4.0);

        /**
         * will be changed if perigee < 156km, see [2.0].
         */
        S = (78.0 / xkmper) + 1.0;

        /**
         * mean motion at epoch [rad/min].
         */
        n_0 = omm.getMeanMotion();

        /**
         * Circulation term in [sec/rev].
         */
        T = SEC_PER_DAY / (n_0 * 1440.0 / TWO_PI);

        /**
         * Large half axis of the orbit in [km].
         */
        large_half_axis = pow(((T / TWOPI) * (T / TWOPI)) * gm, TOTHRD2);

        /**
         * Perigee distance in [km]
         */
        perigee = (large_half_axis * (1.0 - omm.getEccentricity())) - xkmper;

        //----------------------------------------------------------------------
        //---------------------- EQUATIONS from STR #3 -------------------------
        //----------------------------------------------------------------------
        //[1.0]
        a_1 = pow(xke / n_0, TOTHRD);
        //[1.1]
        temp1 = ((3.0 * pow(cos(omm.getInclination()), 2.0) - 1.0) / (pow(
                     1.0 - (omm.getEccentricity() * omm.getEccentricity()), TOTHRD3)));
        delta_1 = TOTHRD3 * (ck2 / (a_1 * a_1)) * temp1;
        //[1.2]
        a_0 = a_1 * (1 - (TOTHRD2 * delta_1) - (delta_1 * delta_1) - ((134.0 / 81.0) * pow(delta_1, 3.0)));
        //[1.3]
        delta_0 = TOTHRD3 * (ck2 / (a_0 * a_0)) * temp1;
        //[1.4]
        n_0dd = n_0 / (1.0 + delta_0);
        //[1.5]
        a_0dd = a_0 / (1.0 - delta_0);

        /**
         * If perigee below 156 km above earth, S and QOMS2T will be changed.
         */
        //[2.0]
        if (perigee < 156.0) {
            if (perigee >= 98.0) {
                s_star = (a_0dd * (1.0 - omm.getEccentricity())) - S + AE;
            } else {
                s_star = (20.0 / xkmper) + AE;
            }
            /**
             * Change QOMS2T.
             */
            QOMS2T = pow(pow(QOMS2T, 0.25) + S - s_star, 4.0);
            /**
             * Change S to s_star.
             */
            S = s_star;
        }

        //----------------------------------------------------------------------
        //---------------------- Calculating the CONSTANTS ---------------------
        //----------------------------------------------------------------------
        //[3.0]
        THETA = cos(omm.getInclination());
        //[3.1]
        XI = 1.0 / (a_0dd - S);
        //[3.2]
        BETA_0 = sqrt(1.0 - (omm.getEccentricity() * omm.getEccentricity()));
        //[3.3]
        ETA = a_0dd * omm.getEccentricity() * XI;
        //[3.4]
        C_2 = QOMS2T * pow(XI, 4) * n_0dd * pow(1.0 - (ETA * ETA), -7.0 / 2.0)
              * (a_0dd * (1.0 + (TOTHRD3 * ETA * ETA) + (4.0 * omm.getEccentricity() * ETA) + (
                              omm.getEccentricity() * pow(ETA, 3.0)))
                 + (TOTHRD3 * ((ck2 * XI) / (1 - (ETA * ETA))) * (-0.5 + (TOTHRD3 * THETA * THETA))
                    * (8.0 + (24.0 * ETA * ETA) + (3.0 * pow(ETA, 4.0)))));
        //[3.5]
        C_1 = omm.getBstar() * C_2;
        //[3.6]
        C_3 = (QOMS2T * pow(XI, 5.0) * a_30 * n_0dd * AE * sin(omm.getInclination())) / (ck2 * omm.getEccentricity());
        //[3.7]
        temp1 = (2.0 * ETA * (1.0 + (omm.getEccentricity() * ETA))) + (0.5 * omm.getEccentricity()) + (
                    0.5 * pow(ETA, 3.0));
        temp2 = (3.0 * (1.0 - (3.0 * THETA * THETA)))
                * (1.0 + (TOTHRD3 * ETA * ETA) - (2 * omm.getEccentricity() * ETA) - (
                       0.5 * omm.getEccentricity() * pow(ETA, 3.0)));
        temp3 = (3.0 / 4.0) * (1.0 - (THETA * THETA))
                * ((2.0 * ETA * ETA) - (omm.getEccentricity() * ETA) - (omm.getEccentricity() * pow(ETA, 3.0))) * cos(
                    2 * omm.getArgumentOfPerigee());

        C_4 = 2.0 * n_0dd * QOMS2T * pow(XI, 4.0) * a_0dd * (BETA_0 * BETA_0) * pow(1.0 - (ETA * ETA), -7.0 / 2.0)
              * (temp1 - ((2.0 * ck2 * XI) / (a_0dd * (1.0 - (ETA * ETA)))) * (temp2 + temp3));
        //[3.8]
        C_5 = 2.0 * QOMS2T * pow(XI, 4.0) * a_0dd * (BETA_0 * BETA_0) * pow(1.0 - (ETA * ETA), -7.0 / 2.0)
              * (1.0 + ((11.0 / 4.0) * ETA * (ETA + omm.getEccentricity())) + (omm.getEccentricity() * pow(ETA, 3.0)));
        //[3.9]
        D_2 = 4.0 * a_0dd * XI * (C_1 * C_1);
        //[3.10]
        D_3 = (4.0 / 3.0) * a_0dd * (XI * XI) * ((17.0 * a_0dd) + S) * pow(C_1, 3.0);
        //[3.11]
        D_4 = TOTHRD * a_0dd * pow(XI, 3.0) * ((221.0 * a_0dd) + (31.0 * S)) * pow(C_1, 4.0);

        neConstsInitialized = true;
    }

    /**
     * Convert time difference in s to time difference in minutes for the
     * calculation procedure.
     */
    timeSinceEpoch = secsAfterEpoch / 60.0;

    //--------------------------------------------------------------------------
    //-------------- The secular effects of atmospheric drag and ---------------
    //-------- gravitation are included through the following equations --------
    //--------------------------------------------------------------------------
    //[4.1]
    temp1 = (3.0 * ck2 * (-1.0 + (3.0 * THETA * THETA))) / (2.0 * a_0dd * a_0dd * pow(BETA_0, 3.0));
    temp2 = (3.0 * ck2 * ck2 * (13.0 - (78.0 * THETA * THETA) + (137.0 * pow(THETA, 4.0))))
            / (16.0 * pow(a_0dd, 4.0) * pow(BETA_0, 7.0));

    M_DF = omm.getMeanAnomaly() + ((1.0 + temp1 + temp2) * (n_0dd * timeSinceEpoch));
    //[4.2]
    temp1 = (-3.0 * ck2 * (1.0 - (5.0 * THETA * THETA))) / (2.0 * a_0dd * a_0dd * pow(BETA_0, 4.0));
    temp2 = (3.0 * ck2 * ck2 * (7.0 - (114.0 * THETA * THETA) + (395.0 * pow(THETA, 4.0)))) / (16.0 * pow(a_0dd, 4.0))
            * (pow(BETA_0, 8.0));
    temp3 = (5.0 * ck4 * (3.0 - (36.0 * THETA * THETA) + (49.0 * pow(THETA, 4.0))))
            / (4.0 * pow(a_0dd, 4.0) * pow(BETA_0, 8.0));

    w_DF = omm.getArgumentOfPerigee() + (temp1 + temp2 + temp3) * n_0dd * timeSinceEpoch;
    //[4.3]
    temp1 = -(3.0 * ck2 * THETA) / (a_0dd * a_0dd * pow(BETA_0, 4.0));
    temp2 = (3.0 * ck2 * ck2 * ((4.0 * THETA) - (19.0 * pow(THETA, 3.0)))) / (2.0 * pow(a_0dd, 4.0) * pow(BETA_0, 8.0));
    temp3 = (5.0 * ck4 * THETA * (3.0 - (7.0 * THETA * THETA))) / (2.0 * pow(a_0dd, 4.0) * pow(BETA_0, 8.0));

    OMEGA_DF = omm.getRaan() + ((temp1 + temp2 + temp3) * n_0dd * timeSinceEpoch);
    //[4.4]
    delta_w = omm.getBstar() * C_3 * cos(omm.getArgumentOfPerigee()) * timeSinceEpoch;
    //[4.5]
    temp1 = pow(1.0 + (ETA * cos(M_DF)), 3.0);
    temp2 = pow(1.0 + (ETA * cos(omm.getMeanAnomaly())), 3.0);

    delta_M = (-TOTHRD * QOMS2T * omm.getBstar() * pow(XI, 4.0) * AE / (omm.getEccentricity() * ETA)) * (temp1 - temp2);

    /**
     * If epoch perigee is less than 220 km, specific terms are
     * dropped/truncated.
     */
    if (perigee < 220.0) {
        //[4.6]
        M_p = M_DF;
        //[4.7]
        w = w_DF;
        //[4.9]
        e = omm.getEccentricity() - (omm.getBstar() * C_4 * timeSinceEpoch);
    } else {
        //[4.6]
        M_p = M_DF + delta_w + delta_M;
        //[4.7]
        w = w_DF - delta_w - delta_M;
        //[4.9]
        e = omm.getEccentricity() - (omm.getBstar() * C_4 * timeSinceEpoch) - (
                omm.getBstar() * C_5 * (sin(M_p) - sin(omm.getMeanAnomaly())));
    }

    //[4.8]
    OMEGA = OMEGA_DF
            - ((21.0 / 2.0) * ((n_0dd * ck2 * THETA) / (a_0dd * a_0dd * BETA_0 * BETA_0)) * C_1 * timeSinceEpoch
               * timeSinceEpoch);

    /**
     * If epoch perigee is less than 220 km, specific terms are dropped/truncated.
     */
    if (perigee < 220.0) {
        //[4.10]
        a = a_0dd * pow(1.0 - (C_1 * timeSinceEpoch), 2.0);
        //[4.11]
        IL = M_p + w + OMEGA + (n_0dd * TOTHRD3 * C_1 * timeSinceEpoch * timeSinceEpoch);
    } else {
        //[4.10]
        a = a_0dd
            * pow(
                1.0 - (C_1 * timeSinceEpoch) - (D_2 * timeSinceEpoch * timeSinceEpoch)
                - (D_3 * pow(timeSinceEpoch, 3.0)) - (D_4 * pow(timeSinceEpoch, 4.0)), 2.0);
        //[4.11]
        temp1 = TOTHRD3 * C_1 * timeSinceEpoch * timeSinceEpoch;
        temp2 = (D_2 + (2.0 * C_1 * C_1)) * pow(timeSinceEpoch, 3.0);
        temp3 = 0.25 * ((3.0 * D_3) + (12.0 * C_1 * D_2) + (10.0 * C_1 * C_1 * C_1)) * pow(timeSinceEpoch, 4.0);
        temp4 = (1.0 / 5.0)
                * ((3.0 * D_4) + (12.0 * C_1 * D_3) + (6.0 * D_2 * D_2) + (30.0 * C_1 * C_1 * D_2)
                   + (15.0 * pow(C_1, 4.0))) * pow(timeSinceEpoch, 5.0);

        IL = M_p + w + OMEGA + (n_0dd * (temp1 + temp2 + temp3 + temp4));
    }

    //[4.12]
    BETA = sqrt(1.0 - (e * e));
    //[4.13]
    n = xke / (pow(a, TOTHRD3));

    //--------------------------------------------------------------------------
    //------------------ The long-period periodic terms ------------------------
    //--------------------------------------------------------------------------
    //[5.0]
    a_xN = e * cos(w);
    //[5.1]
    IL_L = ((a_30 * sin(omm.getInclination())) / (8.0 * ck2 * a * BETA * BETA)) * a_xN * (
               (3.0 + (5.0 * THETA)) / (1.0 + THETA));
    //[5.2]
    a_yNL = ((a_30 * sin(omm.getInclination())) / (4.0 * ck2 * a * BETA * BETA));
    //[5.3]
    IL_T = IL + IL_L;
    //[5.4]
    a_yN = e * sin(w) + a_yNL;

    //--------------------------------------------------------------------------
    //----------------------- Solve Kepler's equation --------------------------
    //--------------------------------------------------------------------------
    //[5.5]
    U = IL_T - OMEGA;
    DELTA_KEPLER = 1.0;
    KEPLER = U;
    int counter = 0;
    /**
     * Fixed according to STR#3 revisited.
     */
    while (fabs(DELTA_KEPLER) >= 1E-12 && counter < 10) {
        DELTA_KEPLER = ((U - (a_yN * cos(KEPLER)) + (a_xN * sin(KEPLER)) - KEPLER)
                        / ((-a_yN * sin(KEPLER)) - (a_xN * cos(KEPLER)) + 1.0));
        KEPLER = KEPLER + DELTA_KEPLER;
        counter++;
    }

    //--------------------------------------------------------------------------
    //------------------- Equations to calculate preliminary -------------------
    //-------------- quantities needed for short-period periodics --------------
    //--------------------------------------------------------------------------
    //[6.0]
    ecosE = a_xN * cos(KEPLER) + a_yN * sin(KEPLER);
    //[6.1]
    esinE = a_xN * sin(KEPLER) - a_yN * cos(KEPLER);
    //[6.2]
    e_L = sqrt((a_xN * a_xN) + (a_yN * a_yN));
    //[6.3]
    p_L = a * (1.0 - (e_L * e_L));
    //[6.4]
    r = a * (1.0 - ecosE);
    //[6.5]
    rDOT = xke * (sqrt(a) / r) * esinE;
    //[6.6]
    rfDOT = xke * (sqrt(p_L) / r);
    //[6.7]
    cosu = (a / r) * (cos(KEPLER) - a_xN + ((a_yN * esinE) / (1.0 + sqrt(1.0 - (e_L * e_L)))));
    //[6.8]
    sinu = (a / r) * (sin(KEPLER) - a_yN - ((a_xN * esinE) / (1.0 + sqrt(1.0 - (e_L * e_L)))));
    //[6.9]
    u = atan2(sinu, cosu);
    sin2u = sin(2 * u);
    cos2u = cos(2 * u);
    //[6.10]
    DELTA_r = (ck2 / (2.0 * p_L)) * (1.0 - (THETA * THETA)) * cos2u;
    //[6.11]
    DELTA_u = (-ck2 / (4.0 * p_L * p_L)) * ((7.0 * THETA * THETA) - 1.0) * sin2u;
    //[6.12]
    DELTA_OMEGA = ((3.0 * ck2 * THETA) / (2.0 * p_L * p_L)) * sin2u;
    //[6.13]
    DELTA_i = ((3.0 * ck2 * THETA) / (2.0 * p_L * p_L)) * sin(omm.getInclination()) * cos2u;
    //[6.14]
    DELTA_rDOT = ((-ck2 * n) / p_L) * (1.0 - (THETA * THETA)) * sin2u;
    //[6.15]
    DELTA_rfDOT = ((ck2 * n) / p_L) * (((1.0 - (THETA * THETA)) * cos2u) - (TOTHRD3 * (1.0 - (3.0 * THETA * THETA))));

    //--------------------------------------------------------------------------
    //------------------ Short-period periodics are added to -------------------
    //--------------------- give the osculating quantities ---------------------
    //--------------------------------------------------------------------------
    //[7.0]
    r_k = (r * (1.0 - (TOTHRD3 * ck2 * (sqrt(1.0 - (e_L * e_L)) / (p_L * p_L))) * ((3.0 * THETA * THETA) - 1.0)))
          + DELTA_r;

    //[7.1]
    u_k = u + DELTA_u;
    //[7.2]
    OMEGA_k = OMEGA + DELTA_OMEGA;
    //[7.3]
    i_k = omm.getInclination() + DELTA_i;
    //[7.4]
    r_kDOT = rDOT + DELTA_rDOT;
    //[7.5]
    rf_kDOT = rfDOT + DELTA_rfDOT;

    //--------------------------------------------------------------------------
    //------------------- Calculate unit orientation vectors -------------------
    //--------------------------------------------------------------------------
    //[8.1]
    M_x = -sin(OMEGA_k) * cos(i_k);
    //[8.2]
    M_y = cos(OMEGA_k) * cos(i_k);
    //[8.3]
    M_z = sin(i_k);
    //[8.4]
    N_x = cos(OMEGA_k);
    //[8.5]
    N_y = sin(OMEGA_k);
    //[8.6]
    N_z = 0;
    //[9.1]
    U_x = (M_x * sin(u_k)) + (N_x * cos(u_k));
    //[9.2]
    U_y = (M_y * sin(u_k)) + (N_y * cos(u_k));
    //[9.3]
    U_z = (M_z * sin(u_k)) + (N_z * cos(u_k));
    //[9.4]
    V_x = (M_x * cos(u_k)) - (N_x * sin(u_k));
    //[9.5]
    V_y = (M_y * cos(u_k)) - (N_y * sin(u_k));
    //[9.6]
    V_z = (M_z * cos(u_k)) - (N_z * sin(u_k));

    //--------------------------------------------------------------------------
    //----------------------- Position and Velocity in ECI ---------------------
    //--------------------------------------------------------------------------
    //[10.1]
    x = r_k * U_x * xkmper;
    //[10.2]
    y = r_k * U_y * xkmper;
    //[10.3]
    z = r_k * U_z * xkmper;
    //[10.4]
    xdot = ((r_kDOT * U_x) + (rf_kDOT * V_x)) * xkmper * MIN_PER_DAY / SEC_PER_DAY;
    //[10.5]
    ydot = ((r_kDOT * U_y) + (rf_kDOT * V_y)) * xkmper * MIN_PER_DAY / SEC_PER_DAY;
    //[10.6]
    zdot = ((r_kDOT * U_z) + (rf_kDOT * V_z)) * xkmper * MIN_PER_DAY / SEC_PER_DAY;

    /**
     * x, y, z in [km], xdot, ydot, zdot in [km/sec]
     */
    satPos.x = x;
    satPos.y = y;
    satPos.z = z;

    satVel.x = xdot;
    satVel.y = ydot;
    satVel.z = zdot;
}
