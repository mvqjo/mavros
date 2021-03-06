/**
 * @brief ActuatorControl plugin
 * @file actuator_control.cpp
 * @author Marcel Stüttgen <stuettgen@fh-aachen.de>
 *
 * @addtogroup plugin
 * @{
 */
/*
 * Copyright 2015 Marcel Stüttgen <stuettgen@fh-aachen.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <mavros/mavros_plugin.h>
#include <pluginlib/class_list_macros.h>

#include <mavros/ActuatorControl.h>

namespace mavplugin {
/**
 * @brief ActuatorControl plugin
 *
 * Sends actuator controls to FCU controller.
 */
class ActuatorControlPlugin : public MavRosPlugin {
public:
	ActuatorControlPlugin() :
		uas(nullptr)
	{ };

	void initialize(UAS &uas_,
			ros::NodeHandle &nh,
			diagnostic_updater::Updater &diag_updater) {
		uas = &uas_;

		actuator_controls_sub = nh.subscribe("actuator_controls", 10, &ActuatorControlPlugin::actuator_control_cb, this);
	}

	const std::string get_name() const {
		return "ActuatorControl";
	}

	const message_map get_rx_handlers() {
		return { /* Rx disabled */ };
	}

private:
	UAS *uas;
	ros::Subscriber actuator_controls_sub;

	/* -*- low-level send -*- */

	//! message definiton here: @p https://pixhawk.ethz.ch/mavlink/#SET_ACTUATOR_CONTROL_TARGET
	void set_actuator_control_target(const uint64_t time_usec,
			const uint8_t group_mix,
			const float controls[8]) {
		mavlink_message_t msg;

		mavlink_msg_set_actuator_control_target_pack_chan(UAS_PACK_CHAN(uas), &msg,
				time_usec,
				group_mix,
				UAS_PACK_TGT(uas),
				controls);
		UAS_FCU(uas)->send_message(&msg);
	}

	/* -*- callbacks -*- */

	void actuator_control_cb(const mavros::ActuatorControl::ConstPtr &req) {
		//! about groups, mixing and channels: @p https://pixhawk.org/dev/mixing
		set_actuator_control_target(ros::Time::now().toNSec() / 1000,
				req->group_mix,
				req->controls.data());
	}
};
};	// namespace mavplugin

PLUGINLIB_EXPORT_CLASS(mavplugin::ActuatorControlPlugin, mavplugin::MavRosPlugin)
