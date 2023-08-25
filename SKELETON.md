[<- Go back](./README.md)
# Entities Skeleton of REGALE (dummy)
🟣 = PUBLISHER DOMAIN PARTITION TOPIC
🟡 = SUBSCRIBER DOMAIN PARTITION TOPIC

- Node manager
🟣P 0 default Monitor_report_job_telemetry
🟣P 0 default Monitor_report_node_telemetry
🟣P 0 default Monitor_report_cluster_telemetry
🟡S 0 default NodeManager_get
🟣P 0 default NodeManager_get_reply
- Monitor
🟡S 0 default Monitor_report_job_telemetry
🟡S 0 default Monitor_report_node_telemetry
🟡S 0 default Monitor_report_cluster_telemetry
- System Power Manage(regale_spm_get_info, regale_spm_get_power, regale_spm_set_powercap)r
🟣P 0 default SystemPowerManager_get
🟣P 0 default SystemPowerManager_set
🟡S 0 default SystemPowerManager_get_reply
🟡S 0 default SystemPowerManager_set_reply
- Job Scheduler (regale_spm_get_info, regale_spm_get_power, regale_spm_set_powercap)
🟣P 0 default SystemPowerManager_get
🟣P 0 default SystemPowerManager_set
🟡S 0 default SystemPowerManager_get_reply
🟡S 0 default SystemPowerManager_set_reply
- Job Manager
- MQTT Broker
