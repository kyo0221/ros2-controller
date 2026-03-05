#include "controller/controller_node.hpp"

namespace controller{

Controller::Controller(const rclcpp::NodeOptions& options) : Controller("", options) {}

Controller::Controller(const std::string& name_space, const rclcpp::NodeOptions& options)
: rclcpp::Node("controller_node", name_space, options),
linear_max_vel(get_parameter("linear_max.vel").as_double()),
angular_max_vel(get_parameter("angular_max.vel").as_double())
{
    _subscription_joy = this->create_subscription<sensor_msgs::msg::Joy>(
        "joy",
        _qos,
        std::bind(&Controller::_subscriber_callback_joy, this, std::placeholders::_1)
    );

    publisher_vel = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", _qos);
    publisher_restart = this->create_publisher<std_msgs::msg::Empty>("restart", _qos);
    publisher_flag = this->create_publisher<std_msgs::msg::Empty>("flag", _qos);
    publisher_autonomous = this->create_publisher<std_msgs::msg::Bool>("autonomous", _qos);

    RCLCPP_INFO(this->get_logger(), "Controller node started.");
}

void Controller::_subscriber_callback_joy(const sensor_msgs::msg::Joy::SharedPtr msg){
    if(upedge_share(msg->buttons[static_cast<int>(Buttons::BACK)])){
        auto msg_autonomous = std::make_shared<std_msgs::msg::Bool>();
        msg_autonomous->data = is_autonomous = !is_autonomous;
        publisher_autonomous->publish(*msg_autonomous);
        RCLCPP_INFO(this->get_logger(), "自動フラグ : %d", msg_autonomous->data);
    }

    if(upedge_share(msg->buttons[static_cast<int>(Buttons::A)])){
        publisher_flag->publish(*std::make_shared<std_msgs::msg::Empty>());
        RCLCPP_INFO(this->get_logger(), "フラグ送信");
    }

    if(upedge_options(msg->buttons[static_cast<int>(Buttons::START)])){
        publisher_restart->publish(*std::make_shared<std_msgs::msg::Empty>());
        RCLCPP_INFO(this->get_logger(), "再稼働");
    }

    if(!is_autonomous){
        auto msg_vel = std::make_shared<geometry_msgs::msg::Twist>();
        msg_vel->linear.x = linear_max_vel * msg->axes[static_cast<int>(Axes::L_y)];
        msg_vel->angular.z = angular_max_vel * msg->axes[static_cast<int>(Axes::R_x)];
        publisher_vel->publish(*msg_vel);
    }
}

}  // namespace controller
