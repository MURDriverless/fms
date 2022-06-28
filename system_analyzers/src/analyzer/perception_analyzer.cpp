#include "system_analyzers/perception_analyzer.h"

PLUGINLIB_EXPORT_CLASS(diagnostic_aggregator::PerceptionAnalyzer,
                       diagnostic_aggregator::Analyzer)

typedef ::mur_common::status_msg_<std::allocator<void>> status_msg;

namespace diagnostic_aggregator
{
    PerceptionAnalyzer::PerceptionAnalyzer() : level_(diagnostic_msgs::DiagnosticStatus::OK) {}

    bool PerceptionAnalyzer::init(const std::string base_path,
                           const ros::NodeHandle &n)
    {
        bool success = GenericAnalyzer::init(base_path, n);
        if (not success)
            return false;

        ros::NodeHandle nh = n;
        status_publisher_ = nh.advertise<status_msg>("/perception_status", 1);

        return true;
    }

    std::vector<diagnostic_msgs::DiagnosticStatusPtr> PerceptionAnalyzer::report()
    {
        std::vector<diagnostic_msgs::DiagnosticStatusPtr> output =
            GenericAnalyzer::report();

        // Find group status
        auto it = std::find_if(
            output.begin(), output.end(),
            [&](const diagnostic_msgs::DiagnosticStatusConstPtr &status)
            {
                return status->name == getPath();
            });

        if (it == output.end())
            return output;

        int8_t level = (*it)->level;
        // Create event when message is matched
        if (level != level_)
        {
            status_msg msg;
            msg.name = "Perception";
            msg.status_code = level;
            status_publisher_.publish(msg);
        }
        level_ = level;

        return output;
    }
}