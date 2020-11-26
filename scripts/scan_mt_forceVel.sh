#!/bin/bash
BASE_NAME="mt_forceVel"
BASE_PARAMS="params_slide.yaml"
echo "Starting ${BASE_NAME} scan"
echo "Base parameter file is ${BASE_PARAMS}"

BASE_SEED=198261346419

for APPLIED_FORCE in 50.0 5.0 0.5 
do
    for I_SEED in 0 1 2 3 4 5 6 7 8 9
    do
        SEED=$(( ${BASE_SEED} + ${I_SEED} ))
        SIM_NAME="${BASE_NAME}_${APPLIED_FORCE}_${I_SEED}"
        PARAM_FILE="params_temp_${SIM_NAME}.yaml"
        echo "Launching sim ${SIM_NAME} with parameter file ${PARAM_FILE}"
        cp ${BASE_PARAMS} ${PARAM_FILE}
        yq w -i ${PARAM_FILE} microtubules.applied_force ${APPLIED_FORCE}
        yq w -i ${PARAM_FILE} seed ${SEED}
        # Run simulation; '&' allows for all to run concurrently 
        ./sim ${PARAM_FILE} ${SIM_NAME} & 
    done
    wait 
    rm params_temp_${BASE_NAME}_*
done