#pragma once

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/bool.hpp>

namespace controller{

// UP / Down Edge
class UpEdge{
public:
    bool operator()(bool value){
        if(!old_value && value){
            old_value=value;
            return true;
        }else{
            old_value=value;
            return false;
        }
    };
private:
    bool old_value = true;
};
class DownEdge{
public:
    bool operator()(bool value){
        if(old_value && !value){
            old_value=value;
            return true;
        }else{
            old_value=value;
            return false;
        }
    };
private:
    bool old_value = false;
};

class Controller : public rclcpp::Node {
public:
    explicit Controller(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

    explicit Controller(const std::string& name_space, const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr _subscription_joy;

    void _subscriber_callback_joy(const sensor_msgs::msg::Joy::SharedPtr msg);

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_vel;
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr publisher_restart;
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr publisher_flag;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr publisher_autonomous;

    rclcpp::QoS _qos = rclcpp::QoS(10);

    const double linear_max_vel;
    const double angular_max_vel;

    bool is_autonomous = false;

    UpEdge upedge_share;
    UpEdge upedge_buttons;
    UpEdge upedge_options;

    enum class Axes{
        L_x,
        L_y,
        L2,
        R_x,
        R_y,
        R2,
        left_and_right,
        up_and_down
    };
    enum class Buttons{
        A,
        B,
        X,
        Y,
        L1,
        R1,
        BACK,
        START,
        PS,
        L3,
        R3
    };
};

}  // namespace controller
