#include <ros/ros.h>
#include <vision_msgs/Detection3DArray.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

visualization_msgs::MarkerArray m_point_markers;

void point_sub_callback(const vision_msgs::Detection3DArray::ConstPtr &msg)
{
    // for (int i = 0; i< m_point_markers.markers.size(); i++)
    // {
    //     m_point_markers.markers.at(i).action = visualization_msgs::Marker::DELETE;
    // }
    m_point_markers.markers.clear();

    if (msg->detections.size() == 0)
    {
        ROS_INFO("No obeject detected.");
        return;
    }

    else 
    {
        const float x_offset = 0.00;     //단위 : 미터
        const float y_offset = -0.06;
        const float z_offset = 0.02;
        const float pi = 3.1415926;
        const float theta = 0.0 * pi / 180; //각도 측정 필요

        for (int i = 0; i < msg->detections.size(); i++)
        {
            //point at camera coordinate
            const float c_point_x = msg->detections.at(i).bbox.center.position.x;
            const float c_point_y = msg->detections.at(i).bbox.center.position.z;
            const float c_point_z = msg->detections.at(i).bbox.center.position.y;
            //point at manipulator coordinate
            const float m_point_x = cos(theta) * c_point_x - sin(theta) * c_point_z + 0.03;
            const float m_point_y = c_point_y + y_offset * 1.5;
            const float m_point_z = sin(theta) * c_point_x + cos(theta) * c_point_z + z_offset; 

            ROS_INFO("coord %d", i);
            ROS_INFO("before translation :  %.3f, %.3f, %.3f  ",  c_point_x, c_point_y, c_point_z);
            ROS_INFO("after transformation : %.3f, %.3f, %.3f  ", m_point_x, m_point_y, m_point_z);

            visualization_msgs::Marker marker;
            marker.header.frame_id = "Fixed frame";
            marker.header.stamp = ros::Time::now();
            marker.type = visualization_msgs::Marker::SPHERE;
            marker.id = i;
            marker.ns = "target object";
            marker.action = visualization_msgs::Marker::ADD;
            marker.pose.position.x = m_point_x;
            marker.pose.position.y = m_point_y;
            marker.pose.position.z = m_point_z;
            marker.pose.orientation.x = 0.0;
            marker.pose.orientation.y = 0.0;
            marker.pose.orientation.z = 0.0;
            marker.pose.orientation.w = 1.0;
            marker.scale.x = 1.0;
            marker.scale.y = 1.0;
            marker.scale.z = 1.0;
            marker.color.r = 1.0f;
            marker.color.g = 0.0f;
            marker.color.b = 0.0f;
            marker.color.a = 1.0;
            marker.lifetime = ros::Duration();

            m_point_markers.markers.push_back(marker);
        }  
    }
}

int main(int argc, char** argv){

    ros::init(argc, argv, "point_visualization_node");
    ros::NodeHandle nh;
    ROS_INFO("==== Point visualization node is started\n");

    ros::Subscriber point_sub = nh.subscribe("/detection_3d/detection_3d", 1, point_sub_callback);
    ros::Publisher transformed_point_pub = nh.advertise<visualization_msgs::MarkerArray>("/transformed_point", 5);
    ros::Rate r(10);


    while(ros::ok())
    {
        ros::spinOnce();
        transformed_point_pub.publish(m_point_markers);
        r.sleep();
    }

    

    return 0;
}
