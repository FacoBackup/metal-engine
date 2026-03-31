#include "EditorCameraRepository.h"
#include "../../common/Icons.h"

namespace Metal {
    void EditorCameraRepository::clear() {}

    void EditorCameraRepository::registerFields() {
        registerEditableField<FLOAT>(&fov).setName("fov").setLabel("Field of view").setMin(1).setMax(179).setIncrement(1.0f);
        registerEditableField<FLOAT>(&zNear).setName("zNear").setLabel("Near plane").setMin(0.001f).setMax(10.0f).setIncrement(0.01f);
        registerEditableField<FLOAT>(&zFar).setName("zFar").setLabel("Far plane").setMin(10.0f).setMax(100000.0f).setIncrement(10.0f);

        registerEditableField<FLOAT>(&rotationSensitivity).setName("rotationSensitivity").setLabel("Rotation sensitivity").setMin(0.01f).setMax(10.0f).setIncrement(0.01f);
        registerEditableField<FLOAT>(&movementSensitivity).setName("movementSensitivity").setLabel("Movement sensitivity").setMin(0.01f).setMax(100.0f).setIncrement(0.1f);
        registerEditableField<FLOAT>(&zoomSensitivity).setName("zoomSensitivity").setLabel("Zoom sensitivity").setMin(0.01f).setMax(100.0f).setIncrement(0.1f);

        registerEditableField<BOOLEAN>(&motionBlurEnabled).setName("motionBlurEnabled").setLabel("Motion blur enabled");
        registerEditableField<FLOAT>(&motionBlurVelocityScale).setName("motionBlurVelocityScale").setLabel("Motion blur velocity scale").setMin(0).setMax(500).setIncrement(1.0f);
        registerEditableField<INT>(&motionBlurMaxSamples).setName("motionBlurMaxSamples").setLabel("Motion blur max samples").setMin(1).setMax(100).setIncrement(1);
        registerEditableField<BOOLEAN>(&cameraMotionBlur).setName("cameraMotionBlur").setLabel("Camera motion blur enabled");

        registerEditableField<BOOLEAN>(&dofEnabled).setName("dofEnabled").setLabel("Depth of field enabled");
        registerEditableField<FLOAT>(&dofFocusDistance).setName("dofFocusDistance").setLabel("Depth of field focus distance").setMin(0.1f).setMax(1000.0f).setIncrement(0.1f);
        registerEditableField<FLOAT>(&dofAperture).setName("dofAperture").setLabel("Depth of field aperture").setMin(0.1f).setMax(64.0f).setIncrement(0.1f);
        registerEditableField<FLOAT>(&dofFocalLength).setName("dofFocalLength").setLabel("Depth of field focal length").setMin(1.0f).setMax(500.0f).setIncrement(1.0f);

        registerEditableField<BOOLEAN>(&bloomEnabled).setName("bloomEnabled").setLabel("Bloom enabled");
        registerEditableField<BOOLEAN>(&filmGrain).setName("filmGrain").setLabel("Film grain enabled");
        registerEditableField<BOOLEAN>(&vignetteEnabled).setName("vignetteEnabled").setLabel("Vignette enabled");
        registerEditableField<BOOLEAN>(&chromaticAberrationEnabled).setName("chromaticAberrationEnabled").setLabel("Chromatic aberration enabled");
        registerEditableField<BOOLEAN>(&distortionEnabled).setName("distortionEnabled").setLabel("Distortion enabled");
        registerEditableField<FLOAT>(&filmGrainStrength).setName("filmGrainStrength").setLabel("Film grain strength").setMin(0.0f).setMax(1.0f).setIncrement(0.01f);
        registerEditableField<FLOAT>(&vignetteStrength).setName("vignetteStrength").setLabel("Vignette strength").setMin(0.0f).setMax(10.0f).setIncrement(0.01f);
        registerEditableField<FLOAT>(&bloomThreshold).setName("bloomThreshold").setLabel("Bloom threshold").setMin(0.0f).setMax(10.0f).setIncrement(0.1f);
        registerEditableField<INT>(&bloomQuality).setName("bloomQuality").setLabel("Bloom quality").setMin(1).setMax(8).setIncrement(1);
        registerEditableField<INT>(&bloomOffset).setName("bloomOffset").setLabel("Bloom offset").setMin(0).setMax(10).setIncrement(1);
        registerEditableField<FLOAT>(&chromaticAberrationIntensity).setName("chromaticAberrationIntensity").setLabel("Chromatic aberration intensity").setMin(0.0f).setMax(1.0f).setIncrement(0.001f);
        registerEditableField<FLOAT>(&distortionIntensity).setName("distortionIntensity").setLabel("Distortion intensity").setMin(-1.0f).setMax(1.0f).setIncrement(0.01f);

        registerSerializableOnlyField<VECTOR3>(&position).setName("position");
        registerSerializableOnlyField<FLOAT>(&pitch).setName("pitch");
        registerSerializableOnlyField<FLOAT>(&yaw).setName("yaw");
    }

    const char *EditorCameraRepository::getIcon() const {
        return Icons::camera_alt.c_str();
    }

    const char *EditorCameraRepository::getTitle() const {
        return "Editor Camera";
    }
}
