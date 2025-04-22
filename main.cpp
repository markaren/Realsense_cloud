#include <iostream>

#include <librealsense2/rs.hpp>

#include <threepp/threepp.hpp>

using namespace threepp;

int main() {
    Canvas canvas;
    GLRenderer renderer(canvas.size());

    auto scene = Scene::create();
    auto camera = PerspectiveCamera::create(60, canvas.aspect(), 0.00001f, 1000.0f);
    camera->position.z = 1;

    OrbitControls controls(*camera, canvas);

    std::pair depth_dim = {320, 240};
    const int numParticles = depth_dim.first * depth_dim.second;
    auto mat = MeshBasicMaterial::create();
    mat->side = Side::Double;
    auto im = InstancedMesh::create(PlaneGeometry::create(0.005, 0.005), mat, numParticles);
    im->rotateZ(math::degToRad(180));
    im->rotateY(math::degToRad(180));

    scene->add(im);

    canvas.onWindowResize([&](WindowSize size) {
        camera->aspect = size.aspect();
        camera->updateProjectionMatrix();
        renderer.setSize(size);
    });

    try {
        // Declare RealSense pipeline, encapsulating the actual device and sensors
        rs2::pipeline p;

        // Configure and start the pipeline
        rs2::config cfg;
        cfg.enable_stream(RS2_STREAM_DEPTH, depth_dim.first, depth_dim.second, RS2_FORMAT_Z16, 30);
        cfg.enable_stream(RS2_STREAM_COLOR, 960 , 540 , RS2_FORMAT_BGR8, 30);

        // Start streaming with default recommended configuration
        p.start(cfg);

        // Point cloud object;
        rs2::pointcloud pc;

        Matrix4 m;
        Color c;

        long long frame = 0;
        canvas.animate([&] {
            // Wait for the next frame set
            rs2::frameset frames = p.wait_for_frames();


            // Get the depth frame
            const auto depth = frames.get_depth_frame();
            const auto color = frames.get_color_frame();

            pc.map_to(color);
            rs2::points points = pc.calculate(depth); // Generate point cloud

            // Get XYZ positions
            const rs2::vertex *vertices = points.get_vertices();
            const rs2::texture_coordinate *tex_coords = points.get_texture_coordinates();

            //Get RGB colors (if color frame is available)
            const auto *color_data = (const uint8_t *) color.get_data();
            int width = color.get_width();
            int height = color.get_height();
            int bytes_per_pixel = color.get_bytes_per_pixel();

            for (size_t i = 0; i < points.size(); i++) {
                m.identity();
                if (vertices[i].z > 0) {
                    m.setPosition(vertices[i].x, vertices[i].y, vertices[i].z);

                    // m.scale(Vector3{0.8,0.8,0.8} * vertices[i].z);

                    // Get texture coordinates in normalized (0,1) range
                    float u = tex_coords[i].u;
                    float v = tex_coords[i].v;

                    // Convert to pixel coordinates
                    int px = std::min(std::max(int(u * width), 0), width - 1);
                    int py = std::min(std::max(int(v * height), 0), height - 1);
                    int color_idx = (py * width + px) * bytes_per_pixel;

                    c.setRGB(color_data[color_idx + 2] / 255.f, color_data[color_idx + 1] / 255.f,
                             color_data[color_idx] / 255.f);
                } else {
                    m.setPosition(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                  std::numeric_limits<float>::max());
                    c.setRGB(0, 0, 0);
                }

                im->setMatrixAt(i, m);
                im->setColorAt(i, c);
            }

            im->instanceColor()->needsUpdate();
            im->instanceMatrix()->needsUpdate();


            renderer.render(*scene, *camera);
        });
    } catch (const rs2::error &e) {
        std::cerr << "RealSense error: " << e.what() << std::endl;
        return 1;
    }
}
